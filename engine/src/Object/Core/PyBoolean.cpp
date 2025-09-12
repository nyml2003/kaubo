#include "Object/Core/PyBoolean.h"
#include "ByteCode/ByteCode.h"
#include "Object/Container/PyList.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr BooleanKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(BooleanKlass::Self())) {
    auto errorMessage = StringConcat(CreatePyList(
      {PyString::Create("AttributeError: '"), obj->Klass()->Name(),
       PyString::Create("' object has no attribute '__repr__'")}
    ));
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  auto boolean = obj->as<PyBoolean>();
  return boolean->Value() ? PyString::Create("True")
                          : PyString::Create("False");
}

PyObjPtr BooleanKlass::str(const PyObjPtr& obj) {
  return repr(obj);
}

PyObjPtr BooleanKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(BooleanKlass::Self()) || !rhs->is(BooleanKlass::Self())) {
    throw std::runtime_error("Boolean does not support eq operation");
  }
  auto left = lhs->as<PyBoolean>();
  auto right = rhs->as<PyBoolean>();
  return PyBoolean::Create(left->Value() == right->Value());
}

PyObjPtr BooleanKlass::_and_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(BooleanKlass::Self())) {
    throw std::runtime_error("Boolean does not support and operation");
  }
  auto left = lhs->as<PyBoolean>();
  auto right = rhs->boolean()->as<PyBoolean>();
  return PyBoolean::Create(left->Value() && right->Value());
}

PyObjPtr BooleanKlass::_or_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(BooleanKlass::Self())) {
    throw std::runtime_error("Boolean does not support or operation");
  }
  auto left = lhs->as<PyBoolean>();
  auto right = rhs->boolean()->as<PyBoolean>();
  return PyBoolean::Create(left->Value() || right->Value());
}

PyObjPtr BooleanKlass::_serialize_(const PyObjPtr& obj) {
  if (!obj->is(BooleanKlass::Self())) {
    throw std::runtime_error("PyBoolean::_serialize_(): obj is not a bool");
  }
  auto boolean = obj->as<PyBoolean>();
  if (boolean->Value()) {
    return PyBytes::Create(Collections::Serialize(Literal::TRUE_LITERAL));
  }
  return PyBytes::Create(Collections::Serialize(Literal::FALSE_LITERAL));
}

}  // namespace kaubo::Object