#pragma once

#include "Object/Core/CoreHelper.h"
#include "Object/Core/IObjectCreator.h"
#include "Object/String/PyString.h"
namespace kaubo::Object {

class MethodKlass : public KlassBase<MethodKlass> {
 public:
  explicit MethodKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(PyString::Create("method")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }
  PyObjPtr repr(const PyObjPtr& obj) override;
};

class PyMethod : public PyObject, public IObjectCreator<PyMethod> {
 public:
  explicit PyMethod(PyObjPtr owner, PyObjPtr method)
    : PyObject(MethodKlass::Self()),
      owner(std::move(owner)),
      method(std::move(method)) {}

  [[nodiscard]] PyObjPtr Method() const { return method; }

  [[nodiscard]] PyObjPtr Owner() const { return owner; }

 private:
  PyObjPtr owner;
  PyObjPtr method;
};
using PyMethodPtr = std::shared_ptr<PyMethod>;

}  // namespace kaubo::Object
