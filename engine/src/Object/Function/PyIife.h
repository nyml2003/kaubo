#pragma once
#include "Object/Core/IObjectCreator.h"
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

class PyIife : public PyObject, public IObjectCreator<PyIife> {
 private:
  TypeFunction nativeFunction;

 public:
  explicit PyIife(TypeFunction nativeFunction)
    : PyObject(IifeKlass::Self()), nativeFunction(std::move(nativeFunction)) {}

  PyObjPtr Call(const PyObjPtr& args) { return nativeFunction(args); }
};


}  // namespace kaubo::Object
