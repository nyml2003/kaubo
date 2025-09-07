#pragma once

#include <functional>
#include "Object/String/PyString.h"

namespace kaubo::Object {

using TypeFunction = std::function<PyObjPtr(PyObjPtr)>;

class NativeFunctionKlass : public KlassBase<NativeFunctionKlass> {
 public:
  explicit NativeFunctionKlass() = default;

  PyObjPtr repr(const PyObjPtr& obj) override;
  void Initialize() override;
};

class PyNativeFunction : public PyObject {
 private:
  TypeFunction nativeFunction;

 public:
  explicit PyNativeFunction(TypeFunction nativeFunction)
    : PyObject(NativeFunctionKlass::Self()),
      nativeFunction(std::move(nativeFunction)) {}

  PyObjPtr Call(const PyObjPtr& args);
};

using PyNativeFunctionPtr = std::shared_ptr<PyNativeFunction>;

inline PyNativeFunctionPtr CreatePyNativeFunction(TypeFunction nativeFunction) {
  return std::make_shared<PyNativeFunction>(std::move(nativeFunction));
}

void CheckNativeFunctionArguments(const PyObjPtr& args);
void CheckNativeFunctionArgumentsWithExpectedLength(
  const PyObjPtr& args,
  Index expected
);

void CheckNativeFunctionArgumentsAtIndexWithType(
  const PyStrPtr& funcName,
  const PyObjPtr& args,
  Index index,
  const KlassPtr& klass
);

void CheckNativeFunctionArgumentWithType(
  const PyStrPtr& funcName,
  const PyObjPtr& arg,
  Index index,
  const KlassPtr& klass
);

}  // namespace kaubo::Object
