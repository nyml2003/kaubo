#pragma once

#include "Object/Function/PyMethod.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Object.h"
#include "Object/Runtime/PyCode.h"
#include "Object/Runtime/PyFrame.h"

namespace kaubo::Runtime {

namespace Evaluator {
Object::PyObjPtr CallNativeFunction(
  const Object::PyNativeFunctionPtr& func,
  const Object::PyListPtr& arguments
);
Object::PyObjPtr
CallMethod(const Object::PyMethodPtr& func, const Object::PyListPtr& arguments);
Object::PyObjPtr InstantiateObject(
  const Object::PyTypePtr& type,
  const Object::PyListPtr& arguments
);
Object::PyObjPtr UserFunction(
  const Object::PyFunctionPtr& func,
  const Object::PyListPtr& arguments
);
Object::PyObjPtr InvokeCallable(
  const Object::PyObjPtr& func,
  const Object::PyListPtr& arguments
);
}  // namespace Evaluator

class VirtualMachine {
 private:
  Object::PyFramePtr frame;
  Object::PyDictPtr builtins;
  explicit VirtualMachine();

 public:
  static VirtualMachine& Instance();

  static void Run(const Object::PyCodePtr& code);

  [[nodiscard]] Object::PyDictPtr Builtins() const;

  void BackToParentFrame();
  void SetFrame(const Object::PyFramePtr& child);
  [[nodiscard]] Object::PyFramePtr CurrentFrame() const;
};

}  // namespace kaubo::Runtime
