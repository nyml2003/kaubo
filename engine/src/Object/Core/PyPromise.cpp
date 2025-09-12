#include <utility>

#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyPromise.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Runtime/EventLoop.h"
#include "Runtime/VirtualMachine.h"

namespace kaubo::Object {

PyPromise::PyPromise(PyObjPtr executor)
  : PyObject(PromiseKlass::Self()),
    state(State::PENDING),
    value(CreatePyNone()),
    onFulfilledCallbacks(CreatePyList()),
    onRejectedCallbacks(CreatePyList()),
    executor(std::move(executor)) {}

PyPromisePtr CreatePyPromise(const PyObjPtr& executor) {
  auto promise = std::make_shared<PyPromise>(executor);
  auto self = promise->shared_from_this()->as<PyPromise>();
  auto resolve = CreatePyNativeFunction([self](const PyObjPtr& args) {
    auto val = args->as<PyList>()->GetItem(0);
    if (self->GetState() == PyPromise::State::PENDING) {
      self->SetState(PyPromise::State::FULFILLED);
      self->SetValue(val);
      ForEach(
        self->GetOnFulfilledCallbacks(),
        [self](const PyObjPtr& callback) {
          Runtime::EventLoop::Instance().EnqueueMicroTask(
            CreatePyNativeFunction([self, callback](const PyObjPtr&) {
              auto nativeCallback = callback->as<PyNativeFunction>();
              nativeCallback->Call(CreatePyList({self->GetValue()}));
              return CreatePyNone();
            })
          );
        }
      );
    }
    return CreatePyNone();
  });
  auto reject = CreatePyNativeFunction([self](const PyObjPtr& args) {
    auto reason = args->as<PyList>()->GetItem(0);
    if (self->GetState() == PyPromise::State::PENDING) {
      self->SetState(PyPromise::State::REJECTED);
      self->SetValue(reason);
      ForEach(self->GetOnRejectedCallbacks(), [self](const PyObjPtr& callback) {
        Runtime::EventLoop::Instance().EnqueueMicroTask(
          CreatePyNativeFunction([self, callback](const PyObjPtr&) {
            auto nativeCallback = callback->as<PyNativeFunction>();
            nativeCallback->Call(CreatePyList({self->GetValue()}));
            return CreatePyNone();
          })
        );
      });
    }
    return CreatePyNone();
  });

  try {
    Runtime::Evaluator::InvokeCallable(
      executor, CreatePyList({resolve, reject})
    );
  } catch (const std::exception& e) {
    reject->Call(CreatePyList({PyString::Create(e.what())}));
  }
  return promise;
}

PyPromisePtr PyPromise::Then(const PyObjPtr& onFulfilled) {
  auto self = shared_from_this()->as<PyPromise>();
  auto new_executor =
    CreatePyNativeFunction([self, onFulfilled](const PyObjPtr& args) {
      auto argList = args->as<PyList>();
      auto resolve = argList->GetItem(0);
      auto reject = argList->GetItem(1);
      auto callback = CreatePyNativeFunction([self, onFulfilled, resolve,
                                              reject](const PyObjPtr&) {
        try {
          auto result = Runtime::Evaluator::InvokeCallable(
            onFulfilled, CreatePyList({self->GetValue()})
          );
          Runtime::Evaluator::InvokeCallable(resolve, CreatePyList({result}));
          return CreatePyNone();
        } catch (const std::exception& e) {
          Runtime::Evaluator::InvokeCallable(
            reject, CreatePyList({PyString::Create(e.what())})
          );
          return CreatePyNone();
        }
      });
      if (self->state == State::FULFILLED) {
        Runtime::EventLoop::Instance().EnqueueMicroTask(callback);
      } else if (self->state == State::REJECTED) {
        Runtime::Evaluator::InvokeCallable(
          reject, CreatePyList({self->GetValue()})
        );
      } else {
        self->onFulfilledCallbacks->Append(callback);
      }
      return CreatePyNone();
    });
  return CreatePyPromise(new_executor);
}

PyPromisePtr PyPromise::Catch(const PyObjPtr& onRejected) {
  auto self = shared_from_this()->as<PyPromise>();
  auto new_executor =
    CreatePyNativeFunction([self, onRejected](const PyObjPtr& args) {
      auto argList = args->as<PyList>();
      auto resolve = argList->GetItem(0);
      auto reject = argList->GetItem(1);
      auto callback = CreatePyNativeFunction([self, onRejected, reject,
                                              resolve](const PyObjPtr&) {
        try {
          auto result = Runtime::Evaluator::InvokeCallable(
            onRejected, CreatePyList({self->GetValue()})
          );
          Runtime::Evaluator::InvokeCallable(resolve, CreatePyList({result}));
          return CreatePyNone();
        } catch (const std::exception& e) {
          Runtime::Evaluator::InvokeCallable(
            reject, CreatePyList({PyString::Create(e.what())})
          );
        }
        return CreatePyNone();
      });
      if (self->state == State::REJECTED) {
        Runtime::EventLoop::Instance().EnqueueMicroTask(callback);
      } else {
        self->onRejectedCallbacks->Append(callback);
      }
      return CreatePyNone();
    });
  return CreatePyPromise(new_executor);
}

void PromiseKlass::Initialize() {
  if (IsInitialized()) {
    return;
  }
  InitKlass(PyString::Create("Promise")->as<PyString>(), Self());

  AddAttribute(
    PyString::Create("then"),
    CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto promise = argList->GetItem(0)->as<PyPromise>();
      auto onFulfilled = argList->GetItem(1);
      return promise->Then(onFulfilled);
    })
  );
  AddAttribute(
    PyString::Create("catch"),
    CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto promise = argList->GetItem(0)->as<PyPromise>();
      auto onRejected = argList->GetItem(1);
      return promise->Catch(onRejected);
    })
  );
  AddAttribute(
    PyString::Create("resolve"), CreatePyNativeFunction(PromiseResolve)
  );
  AddAttribute(
    PyString::Create("reject"), CreatePyNativeFunction(PromiseReject)
  );

  SetInitialized();
}

PyObjPtr PromiseKlass::init(const PyObjPtr& /*typeObj*/, const PyObjPtr& args) {
  if (args->as<PyList>()->Length() != 1) {
    throw std::runtime_error("Promise constructor requires one argument");
  }
  auto executor = args->as<PyList>()->GetItem(0);
  return CreatePyPromise(executor);
}

auto PromiseResolve(const PyObjPtr& args) -> PyObjPtr {
  auto argList = args->as<PyList>();
  auto value = argList->GetItem(0);
  if (value->Klass() == PromiseKlass::Self()) {
    return value;
  }
  auto executor = CreatePyNativeFunction([value](const PyObjPtr& args) {
    auto argList = args->as<PyList>();
    auto resolve = argList->GetItem(0);
    return Runtime::Evaluator::InvokeCallable(resolve, CreatePyList({value}));
  });
  return CreatePyPromise(executor);
}

auto PromiseReject(const PyObjPtr& args) -> PyObjPtr {
  auto argList = args->as<PyList>();
  auto reason = argList->GetItem(0);
  auto executor = CreatePyNativeFunction([reason](const PyObjPtr& args) {
    auto argList = args->as<PyList>();
    auto reject = argList->GetItem(0);
    return Runtime::Evaluator::InvokeCallable(reject, CreatePyList({reason}));
  });
  return CreatePyPromise(executor);
}

}  // namespace kaubo::Object