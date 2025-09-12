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
    value(PyNone::Create()),
    onFulfilledCallbacks(PyList::Create()),
    onRejectedCallbacks(PyList::Create()),
    executor(std::move(executor)) {}

PyPromisePtr CreatePyPromise(const PyObjPtr& executor) {
  auto promise = std::make_shared<PyPromise>(executor);
  auto self = promise->shared_from_this()->as<PyPromise>();
  auto resolve = PyNativeFunction::Create([self](const PyObjPtr& args) {
    auto val = args->as<PyList>()->GetItem(0);
    if (self->GetState() == PyPromise::State::PENDING) {
      self->SetState(PyPromise::State::FULFILLED);
      self->SetValue(val);
      ForEach(
        self->GetOnFulfilledCallbacks(),
        [self](const PyObjPtr& callback) {
          Runtime::EventLoop::Instance().EnqueueMicroTask(
            PyNativeFunction::Create([self, callback](const PyObjPtr&) {
              auto nativeCallback = callback->as<PyNativeFunction>();
              nativeCallback->Call(
                PyList::Create<Object::PyObjPtr>({self->GetValue()})
              );
              return PyNone::Create();
            })
          );
        }
      );
    }
    return PyNone::Create();
  });
  auto reject = PyNativeFunction::Create([self](const PyObjPtr& args) {
    auto reason = args->as<PyList>()->GetItem(0);
    if (self->GetState() == PyPromise::State::PENDING) {
      self->SetState(PyPromise::State::REJECTED);
      self->SetValue(reason);
      ForEach(self->GetOnRejectedCallbacks(), [self](const PyObjPtr& callback) {
        Runtime::EventLoop::Instance().EnqueueMicroTask(
          PyNativeFunction::Create([self, callback](const PyObjPtr&) {
            auto nativeCallback = callback->as<PyNativeFunction>();
            nativeCallback->Call(
              PyList::Create<Object::PyObjPtr>({self->GetValue()})
            );
            return PyNone::Create();
          })
        );
      });
    }
    return PyNone::Create();
  });

  try {
    Runtime::Evaluator::InvokeCallable(
      executor, PyList::Create<Object::PyObjPtr>({resolve, reject})
    );
  } catch (const std::exception& e) {
    reject->Call(
      PyList::Create<Object::PyObjPtr>({PyString::Create(e.what())})
    );
  }
  return promise;
}

PyPromisePtr PyPromise::Then(const PyObjPtr& onFulfilled) {
  auto self = shared_from_this()->as<PyPromise>();
  auto new_executor =
    PyNativeFunction::Create([self, onFulfilled](const PyObjPtr& args) {
      auto argList = args->as<PyList>();
      auto resolve = argList->GetItem(0);
      auto reject = argList->GetItem(1);
      auto callback = PyNativeFunction::Create([self, onFulfilled, resolve,
                                                reject](const PyObjPtr&) {
        try {
          auto result = Runtime::Evaluator::InvokeCallable(
            onFulfilled, PyList::Create<Object::PyObjPtr>({self->GetValue()})
          );
          Runtime::Evaluator::InvokeCallable(resolve, PyList::Create({result}));
          return PyNone::Create();
        } catch (const std::exception& e) {
          Runtime::Evaluator::InvokeCallable(
            reject,
            PyList::Create<Object::PyObjPtr>({PyString::Create(e.what())})
          );
          return PyNone::Create();
        }
      });
      if (self->state == State::FULFILLED) {
        Runtime::EventLoop::Instance().EnqueueMicroTask(callback);
      } else if (self->state == State::REJECTED) {
        Runtime::Evaluator::InvokeCallable(
          reject, PyList::Create<Object::PyObjPtr>({self->GetValue()})
        );
      } else {
        self->onFulfilledCallbacks->Append(callback);
      }
      return PyNone::Create();
    });
  return CreatePyPromise(new_executor);
}

PyPromisePtr PyPromise::Catch(const PyObjPtr& onRejected) {
  auto self = shared_from_this()->as<PyPromise>();
  auto new_executor =
    PyNativeFunction::Create([self, onRejected](const PyObjPtr& args) {
      auto argList = args->as<PyList>();
      auto resolve = argList->GetItem(0);
      auto reject = argList->GetItem(1);
      auto callback = PyNativeFunction::Create([self, onRejected, reject,
                                                resolve](const PyObjPtr&) {
        try {
          auto result = Runtime::Evaluator::InvokeCallable(
            onRejected, PyList::Create<Object::PyObjPtr>({self->GetValue()})
          );
          Runtime::Evaluator::InvokeCallable(
            resolve, PyList::Create<Object::PyObjPtr>({result})
          );
          return PyNone::Create();
        } catch (const std::exception& e) {
          Runtime::Evaluator::InvokeCallable(
            reject,
            PyList::Create<Object::PyObjPtr>({PyString::Create(e.what())})
          );
        }
        return PyNone::Create();
      });
      if (self->state == State::REJECTED) {
        Runtime::EventLoop::Instance().EnqueueMicroTask(callback);
      } else {
        self->onRejectedCallbacks->Append(callback);
      }
      return PyNone::Create();
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
    PyNativeFunction::Create([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto promise = argList->GetItem(0)->as<PyPromise>();
      auto onFulfilled = argList->GetItem(1);
      return promise->Then(onFulfilled);
    })
  );
  AddAttribute(
    PyString::Create("catch"),
    PyNativeFunction::Create([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto promise = argList->GetItem(0)->as<PyPromise>();
      auto onRejected = argList->GetItem(1);
      return promise->Catch(onRejected);
    })
  );
  AddAttribute(
    PyString::Create("resolve"), PyNativeFunction::Create(PromiseResolve)
  );
  AddAttribute(
    PyString::Create("reject"), PyNativeFunction::Create(PromiseReject)
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
  auto executor = PyNativeFunction::Create([value](const PyObjPtr& args) {
    auto argList = args->as<PyList>();
    auto resolve = argList->GetItem(0);
    return Runtime::Evaluator::InvokeCallable(
      resolve, PyList::Create<Object::PyObjPtr>({value})
    );
  });
  return CreatePyPromise(executor);
}

auto PromiseReject(const PyObjPtr& args) -> PyObjPtr {
  auto argList = args->as<PyList>();
  auto reason = argList->GetItem(0);
  auto executor = PyNativeFunction::Create([reason](const PyObjPtr& args) {
    auto argList = args->as<PyList>();
    auto reject = argList->GetItem(0);
    return Runtime::Evaluator::InvokeCallable(
      reject, PyList::Create<Object::PyObjPtr>({reason})
    );
  });
  return CreatePyPromise(executor);
}

}  // namespace kaubo::Object