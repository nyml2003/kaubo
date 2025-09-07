#include "Runtime/VirtualMachine.h"
#include "Function/BuiltinFunction.h"
#include "Object/Container/PyDictionary.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyFunction.h"
#include "Object/Function/PyMethod.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Iterator/PyGenerator.h"
#include "Object/Object.h"
#include "Object/Runtime/PyCode.h"
#include "Object/Runtime/PyFrame.h"
#include "Runtime/EventLoop.h"
#include "Runtime/GarbageCollector.h"
#include "Runtime/Genesis.h"

namespace kaubo::Runtime {

VirtualMachine::VirtualMachine() {
  frame = nullptr;
  builtins = Genesis();
  // // 初始化垃圾回收器
  // GarbageCollector::GetInstance();
}

VirtualMachine& VirtualMachine::Instance() {
  static VirtualMachine instance;
  return instance;
}

Object::PyFramePtr VirtualMachine::CurrentFrame() const {
  return frame;
}

void VirtualMachine::SetFrame(const Object::PyFramePtr& child) {
  frame = child;
}

void VirtualMachine::Run(const Object::PyCodePtr& code) {
  auto result = CreateModuleEntryFrame(code)->EvalAndDestroy();
  if (!result->is(Object::NoneKlass::Self())) {
    throw std::runtime_error("Module code did not return None");
  }
  EventLoop::Instance().Run();
}

namespace Evaluator {
Object::PyObjPtr CallNativeFunction(
  const Object::PyNativeFunctionPtr& func,
  const Object::PyListPtr& arguments
) {
  return func->Call(arguments);
}
Object::PyObjPtr CallMethod( // NOLINT(misc-no-recursion)
  const Object::PyMethodPtr& func,
  const Object::PyListPtr& arguments
) {
  auto owner = func->Owner();
  auto function = func->Method();
  return InvokeCallable(
    function, arguments->Prepend(owner)->as<Object::PyList>()
  );
}
Object::PyObjPtr InstantiateObject(
  const Object::PyTypePtr& type,
  const Object::PyListPtr& arguments
) {
  return type->Owner()->init(type, arguments);
}
Object::PyObjPtr UserFunction(
  const Object::PyFunctionPtr& func,
  const Object::PyListPtr& arguments
) {
  auto frame = CreateFrameWithPyFunction(func, arguments);
  if (func->Code()->IsGenerator()) {
    Runtime::VirtualMachine::Instance().BackToParentFrame();
    return Object::CreatePyGenerator(frame);
  }
  return frame->EvalAndDestroy();
}
Object::PyObjPtr InvokeCallable( // NOLINT( misc-no-recursion)
  const Object::PyObjPtr& func,
  const Object::PyListPtr& arguments
) {
  if (func->is(Object::MethodKlass::Self())) {
    auto method = func->as<Object::PyMethod>();
    return CallMethod(method, arguments);
  }
  if (func->is(Object::FunctionKlass::Self())) {
    auto pyFunction = func->as<Object::PyFunction>();
    return UserFunction(pyFunction, arguments);
  }
  if (func->is(Object::NativeFunctionKlass::Self())) {
    auto nativeFunction = func->as<Object::PyNativeFunction>();
    return CallNativeFunction(nativeFunction, arguments);
  }
  if (func->is(Object::TypeKlass::Self())) {
    auto type = func->as<Object::PyType>();
    return InstantiateObject(type, arguments);
  }
  Function::DebugPrint(func);
  throw std::runtime_error("Unknown function type");
}
}  // namespace Evaluator

Object::PyDictPtr VirtualMachine::Builtins() const {
  return builtins;
}

void VirtualMachine::BackToParentFrame() {
  if (!frame->is(Object::FrameKlass::Self())) {
    throw std::runtime_error("Cannot destroy non-frame object");
  }
  frame = frame->as<Object::PyFrame>()->Caller();
}

}  // namespace kaubo::Runtime