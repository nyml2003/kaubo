#pragma once

#include "IObjectCreator.h"
#include "Object/Core/CoreHelper.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class PyBoolean;
using PyBoolPtr = std::shared_ptr<PyBoolean>;

class BooleanKlass : public KlassBase<BooleanKlass> {
 public:
  explicit BooleanKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(PyString::Create("bool")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override;
  PyObjPtr boolean(const PyObjPtr& obj) override { return obj; }
  PyObjPtr _and_(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr _or_(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr _serialize_(const PyObjPtr& obj) override;
};

class PyBoolean : public PyObject, public IObjectCreator<PyBoolean> {
 private:
  bool value;

 public:
  explicit PyBoolean(bool value)
    : PyObject(BooleanKlass::Self()), value(value) {}
  static PyBoolPtr False() {
    static PyBoolPtr instance = IObjectCreator::Create(false);
    return instance;
  }
  static PyBoolPtr True() {
    static PyBoolPtr instance = IObjectCreator::Create(true);
    return instance;
  }
  bool Value() const { return value; }

  static PyBoolPtr Create(bool value) {
    if (value) {
      return PyBoolean::True();
    }
    return PyBoolean::False();
  }
};

inline bool IsTrue(const PyObjPtr& obj) {
  return obj->boolean()->as<PyBoolean>()->Value();
}
inline PyObjPtr Not(const PyObjPtr& obj) {
  return IsTrue(obj) ? PyBoolean::False() : PyBoolean::True();
}
inline PyObjPtr And(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return IsTrue(lhs) && IsTrue(rhs) ? PyBoolean::True() : PyBoolean::False();
}
inline PyObjPtr Or(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return IsTrue(lhs) || IsTrue(rhs) ? PyBoolean::True() : PyBoolean::False();
}
}  // namespace kaubo::Object
