
#include <memory>
// NOLINTBEGIN(*)
#include "../test_default.h"

#include "Function/BuiltinFunction.h"
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyPromise.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Number/PyInteger.h"
#include "Runtime/EventLoop.h"

using namespace kaubo::Object;       // NOLINT
using namespace kaubo::Collections;  // NOLINT
using namespace kaubo::Runtime;      // NOLINT

namespace {
constexpr int64_t TEST_VALUE_1 = 100;
constexpr int64_t TEST_VALUE_2 = 2;
constexpr int64_t TEST_VALUE_3 = 5;
}  // namespace

class EventLoopTest : public ::testing::Test {
 private:
  void SetUp() override {
    LoadBootstrapClasses();
    LoadRuntimeSupportClasses();
  }
};

// TEST_F(EventLoopTest, BasicTaskExecution) {
//   bool executed = false;
//   EventLoop::Instance().EnqueueTask([this, &executed]() {
//     executed = true;
//     EventLoop::Instance().Stop();
//   });

//   EventLoop::Instance().Run();
//   EXPECT_TRUE(executed);
// }

TEST_F(EventLoopTest, PromiseChaining) {
  std::thread taskThread([]() { EventLoop::Instance().Run(); });
  auto executer = CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
    auto argList = args->as<PyList>();
    auto resolve = argList->GetItem(0)->as<PyNativeFunction>();
    auto value = CreatePyInteger(TEST_VALUE_1);
    resolve->Call(CreatePyList({value}));
    kaubo::Function::DebugPrint(value);
    return CreatePyNone();
  });
  auto promise = CreatePyPromise(executer);
  auto promise2 =
    promise->Then(CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto value = argList->GetItem(0);
      value = value->mul(CreatePyInteger(TEST_VALUE_2));
      kaubo::Function::DebugPrint(value);
      return value;
    }));
  auto promise3 =
    promise2->Then(CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto value = argList->GetItem(0);
      value = value->add(CreatePyInteger(TEST_VALUE_3));
      kaubo::Function::DebugPrint(value);
      return value;
    }));
  auto promise4 =
    promise3->Then(CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto value = argList->GetItem(0);
      throw std::runtime_error("Test error");
      return CreatePyNone();
    }));
  auto promise5 = promise4->Catch(
    CreatePyNativeFunction([](const PyObjPtr& args) -> PyObjPtr {
      auto argList = args->as<PyList>();
      auto error = argList->GetItem(0)->as<PyString>();
      kaubo::Function::DebugPrint(error);
      return CreatePyNone();
    })
  );
  kaubo::Function::DebugPrint(CreatePyInteger(114ULL));
  EventLoop::Instance().EnqueueTask(CreatePyNativeFunction([](const PyObjPtr&) {
    if (EventLoop::Instance().Idle()) {
      EventLoop::Instance().Stop();
    }
    return CreatePyNone();
  }));
  taskThread.join();
}

// TEST_F(EventLoopTest, COUNT100) {
//   for (int i = 0; i < 5; i++) {
//     EventLoop::Instance().EnqueueTask([i]() {
//       std::cout << "Task " << i << " executed." << std::endl;
//     });
//   }
//   for (int i = 0; i < 5; i++) {
//     EventLoop::Instance().EnqueueMicroTask([i]() {
//       std::cout << "Micro Task " << i << " executed." << std::endl;
//     });
//   }
//   EventLoop::Instance().EnqueueTask([]() {
//     if (EventLoop::Instance().Idle()) {
//       EventLoop::Instance().Stop();
//     }
//   });
//   EventLoop::Instance().Run();
// }
// NOLINTEND(*)