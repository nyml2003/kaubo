#include "Object/Core/PyNone.h"
#include "ByteCode/ByteCode.h"
#include "Object/Core/PyBoolean.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr NoneKlass::_serialize_(const PyObjPtr& obj) {
  if (!obj->is(NoneKlass::Self())) {
    throw std::runtime_error("NoneType does not support serialization");
  }
  return CreatePyBytes(Collections::Serialize(Literal::NONE));
}

PyObjPtr NoneKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(NoneKlass::Self())) {
    throw std::runtime_error("NoneType does not support repr operation");
  }
  return CreatePyString("None");
}

PyObjPtr NoneKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (lhs->is(NoneKlass::Self()) && rhs->is(NoneKlass::Self())) {
    return CreatePyBoolean(true);
  }
  return CreatePyBoolean(false);
}

PyObjPtr NoneKlass::boolean(const PyObjPtr& obj) {
  if (!obj->is(NoneKlass::Self())) {
    throw std::runtime_error("NoneType does not support boolean operation");
  }
  return CreatePyBoolean(false);
}

PyNone::PyNone() : PyObject(NoneKlass::Self()) {}

PyNonePtr PyNone::Instance() {
  static PyNonePtr instance = std::make_shared<PyNone>();
  return instance;
}

}  // namespace kaubo::Object