#pragma once

#include "Object/Core/CoreHelper.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class NoneKlass : public KlassBase<NoneKlass> {
 public:
  explicit NoneKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("NoneType")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override { return repr(obj); }
  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr boolean(const PyObjPtr& obj) override;

  PyObjPtr _serialize_(const PyObjPtr& obj) override;
};
class PyNone;
using PyNonePtr = std::shared_ptr<PyNone>;
class PyNone : public PyObject {
 public:
  explicit PyNone();
  static PyNonePtr Instance();
};

inline PyNonePtr CreatePyNone() {
  return PyNone::Instance();
}

}  // namespace kaubo::Object
