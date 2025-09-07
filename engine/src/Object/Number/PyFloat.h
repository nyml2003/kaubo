#pragma once

#include "Object/Core/CoreHelper.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class FloatKlass : public KlassBase<FloatKlass> {
 public:
  explicit FloatKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("float")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr init(const PyObjPtr& klass, const PyObjPtr& args) override;
  PyObjPtr add(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr sub(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr mul(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr floordiv(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr truediv(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr mod(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr pow(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override { return repr(obj); }
  PyObjPtr hash(const PyObjPtr& obj) override;
  PyObjPtr neg(const PyObjPtr& obj) override;
  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr lt(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr boolean(const PyObjPtr& obj) override;

  PyObjPtr _serialize_(const PyObjPtr& obj) override;
};

class PyFloat : public PyObject {
 private:
  double value;

 public:
  explicit PyFloat(double value) : PyObject(FloatKlass::Self()), value(value) {}

  [[nodiscard]] double Value() const { return value; }
};
using PyFloatPtr = std::shared_ptr<PyFloat>;
inline PyFloatPtr CreatePyFloat(double value) {
  return std::make_shared<PyFloat>(value);
}

}  // namespace kaubo::Object

