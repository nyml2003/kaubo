#include "Object/Number/PyFloat.h"
#include "ByteCode/ByteCode.h"
#include "Collections/Integer/IntegerHelper.h"
#include "Collections/String/BytesHelper.h"
#include "Collections/String/StringHelper.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyType.h"
#include "Object/Number/PyInteger.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr FloatKlass::init(const PyObjPtr& klass, const PyObjPtr& args) {
  if (klass->as<PyType>()->Owner() != Self()) {
    throw std::runtime_error("PyFloat::init(): klass is not a float");
  }
  auto argList = args->as<PyList>();
  if (argList->Length() == 0) {
    return CreatePyFloat(0.0);
  }
  if (argList->Length() != 1) {
    throw std::runtime_error(
      "PyFloat::init(): args must be a list with one element"
    );
  }
  auto value = argList->GetItem(0);
  if (value->is(IntegerKlass::Self())) {
    return CreatePyFloat(static_cast<double>(value->as<PyInteger>()->ToU64()));
  }
  if (!value->is(Self())) {
    throw std::runtime_error("PyFloat::init(): value is not a float");
  }
  return value;
}

PyObjPtr FloatKlass::add(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::add(): lhs or rhs is not a float");
  }
  return CreatePyFloat(
    lhs->as<PyFloat>()->Value() + rhs->as<PyFloat>()->Value()
  );
}

PyObjPtr FloatKlass::sub(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::sub(): lhs or rhs is not a float");
  }
  return CreatePyFloat(
    lhs->as<PyFloat>()->Value() - rhs->as<PyFloat>()->Value()
  );
}

PyObjPtr FloatKlass::mul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self())) {
    throw std::runtime_error("PyFloat::mul(): lhs is not a float");
  }
  if (rhs->is(Self())) {
    return CreatePyFloat(
      lhs->as<PyFloat>()->Value() * rhs->as<PyFloat>()->Value()
    );
  }
  if (rhs->is(IntegerKlass::Self())) {
    return CreatePyFloat(
      lhs->as<PyFloat>()->Value() *
      static_cast<double>(rhs->as<PyInteger>()->ToU64())
    );
  }
  throw std::runtime_error("PyFloat::mul(): rhs is not a float or int");
}

PyObjPtr FloatKlass::truediv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::div(): lhs or rhs is not a float");
  }
  return CreatePyFloat(
    lhs->as<PyFloat>()->Value() / rhs->as<PyFloat>()->Value()
  );
}

PyObjPtr FloatKlass::floordiv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::floordiv(): lhs or rhs is not a float");
  }
  return CreatePyFloat(
    std::floor(lhs->as<PyFloat>()->Value() / rhs->as<PyFloat>()->Value())
  );
}

PyObjPtr FloatKlass::mod(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::mod(): lhs or rhs is not a float");
  }
  return CreatePyFloat(
    std::fmod(lhs->as<PyFloat>()->Value(), rhs->as<PyFloat>()->Value())
  );
}

PyObjPtr FloatKlass::pow(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::pow(): lhs or rhs is not a float");
  }
  return CreatePyFloat(
    std::pow(lhs->as<PyFloat>()->Value(), rhs->as<PyFloat>()->Value())
  );
}

PyObjPtr FloatKlass::hash(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("PyFloat::hash(): obj is not a float");
  }
  return CreatePyInteger(
    Collections::CreateIntegerWithU64(
      static_cast<uint64_t>(obj->as<PyFloat>()->Value())
    )
  );
}

PyObjPtr FloatKlass::neg(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("PyFloat::neg(): obj is not a float");
  }
  return CreatePyFloat(-obj->as<PyFloat>()->Value());
}

PyObjPtr FloatKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("PyFloat::repr(): obj is not a float");
  }
  auto floatObj = obj->as<PyFloat>();
  return CreatePyString(Collections::ToString(floatObj->Value()));
}

PyObjPtr FloatKlass::_serialize_(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("PyFloat::_serialize_(): obj is not a float");
  }
  auto floatObj = obj->as<PyFloat>();
  Collections::StringBuilder stringBuilder(
    Collections::Serialize(Literal::FLOAT)
  );
  stringBuilder.Append(Collections::Serialize(floatObj->Value()));
  return CreatePyBytes(stringBuilder.ToString());
}

PyObjPtr FloatKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::eq(): lhs or rhs is not a float");
  }
  return PyBoolean::create(
    lhs->as<PyFloat>()->Value() == rhs->as<PyFloat>()->Value()
  );
}

PyObjPtr FloatKlass::lt(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("PyFloat::lt(): lhs or rhs is not a float");
  }
  return PyBoolean::create(
    lhs->as<PyFloat>()->Value() < rhs->as<PyFloat>()->Value()
  );
}

PyObjPtr FloatKlass::boolean(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("PyFloat::boolean(): obj is not a float");
  }
  return PyBoolean::create(obj->as<PyFloat>()->Value() != 0.0);
}

}  // namespace kaubo::Object
