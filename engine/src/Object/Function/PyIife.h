#pragma once
#include "Object/Core/PyObject.h"

#include <functional>

namespace kaubo::Object {

using TypeFunction = std::function<PyObjPtr(PyObjPtr)>;

class IifeKlass : public KlassBase<IifeKlass> {
 public:
  explicit IifeKlass() = default;

  void Initialize() override;

  PyObjPtr str(const PyObjPtr& self) override;
};

class PyIife : public PyObject {
 private:
  TypeFunction nativeFunction;

 public:
  explicit PyIife(TypeFunction nativeFunction)
    : PyObject(IifeKlass::Self()), nativeFunction(std::move(nativeFunction)) {}

  PyObjPtr Call(const PyObjPtr& args) { return nativeFunction(args); }
};

inline PyObjPtr CreatePyIife(TypeFunction nativeFunction) {
  return std::make_shared<PyIife>(std::move(nativeFunction));
}

}  // namespace kaubo::Object
