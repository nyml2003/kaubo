#include "Object/Core/Klass.h"
#include "Function/BuiltinFunction.h"
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyIife.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Runtime/VirtualMachine.h"

namespace kaubo::Object {

void Klass::SetName(const PyStrPtr& _name) {
  this->name = _name;
}

void Klass::SetAttributes(const PyDictPtr& _attributes) {
  this->attributes = _attributes;
}

void Klass::SetType(const PyTypePtr& _type) {
  this->type = _type;
}

void Klass::SetSuper(const PyListPtr& _super) {
  this->super = _super;
}

void Klass::SetMro(const PyListPtr& _mro) {
  this->mro = _mro;
}

PyObjPtr Klass::init(const PyObjPtr& typeObj, const PyObjPtr& args) {
  auto* instanceType = typeObj->as<PyType>()->Owner();
  auto instance = std::make_shared<PyObject>(instanceType);
  if (instanceType->IsNative()) {
    return instance;
  }
  Invoke(instance, PyString::Create("__init__"), args->as<PyList>());
  return instance;
}

PyObjPtr Klass::add(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__add__"), CreatePyList({rhs}));
}

PyObjPtr Klass::sub(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__sub__"), CreatePyList({rhs}));
}

PyObjPtr Klass::mul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__mul__"), CreatePyList({rhs}));
}

PyObjPtr Klass::floordiv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__floordiv__"), CreatePyList({rhs}));
}

PyObjPtr Klass::truediv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__truediv__"), CreatePyList({rhs}));
}

PyObjPtr Klass::pos(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__pos__"), {});
}

PyObjPtr Klass::neg(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__neg__"), {});
}

PyObjPtr Klass::invert(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__invert__"), {});
}

PyObjPtr Klass::_and_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__and__"), CreatePyList({rhs}));
}

PyObjPtr Klass::_or_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__or__"), CreatePyList({rhs}));
}

PyObjPtr Klass::_xor_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__xor__"), CreatePyList({rhs}));
}

PyObjPtr Klass::lshift(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__lshift__"), CreatePyList({rhs}));
}

PyObjPtr Klass::rshift(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__rshift__"), CreatePyList({rhs}));
}

PyObjPtr Klass::mod(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__mod__"), CreatePyList({rhs}));
}

PyObjPtr Klass::divmod(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__divmod__"), CreatePyList({rhs}));
}

PyObjPtr Klass::pow(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__pow__"), CreatePyList({rhs}));
}

PyObjPtr Klass::repr(const PyObjPtr& self) {
  if (isNative) {
    return StringConcat(CreatePyList(
      {PyString::Create("<"),
       self->getattr(PyString::Create("__name__")->as<PyString>()),
       PyString::Create(" object at "),
       Function::Identity(CreatePyList({self})), PyString::Create(">")}
    ));
  }
  auto reprFunc = GetAttr(self, PyString::Create("__repr__")->as<PyString>());
  if (reprFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(reprFunc, CreatePyList({self}));
  }
  return StringConcat(CreatePyList(
    {PyString::Create("<"),
     self->getattr(PyString::Create("__name__")->as<PyString>()),
     PyString::Create(" object at "), Function::Identity(CreatePyList({self})),
     PyString::Create(">")}
  ));
}

PyObjPtr Klass::hash(const PyObjPtr& obj) {
  if (obj->Hashed()) {
    return CreatePyInteger(obj->HashValue());
  }
  auto hashFunc = obj->getattr(PyString::Create("__hash__")->as<PyString>());
  if (hashFunc != nullptr) {
    auto pyHashValue =
      Runtime::Evaluator::InvokeCallable(hashFunc, CreatePyList());
    if (!pyHashValue->is(IntegerKlass::Self())) {
      throw std::runtime_error("Hash value must be an integer");
    }
    auto hashValue = reinterpret_cast<uint64_t>(obj.get());
    obj->SetHashValue(hashValue);
    return CreatePyInteger(hashValue);
  }
  auto hashValue = reinterpret_cast<uint64_t>(obj.get());
  obj->SetHashValue(hashValue);
  return CreatePyInteger(hashValue);
}

PyObjPtr Klass::gt(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Not(lhs->le(rhs));
}

PyObjPtr Klass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__eq__"), CreatePyList({rhs}));
}

PyObjPtr Klass::lt(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__lt__"), CreatePyList({rhs}));
}

PyObjPtr Klass::ge(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Not(lhs->lt(rhs));
}

PyObjPtr Klass::le(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Or(lhs->lt(rhs), lhs->eq(rhs));
}

PyObjPtr Klass::ne(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Not(lhs->eq(rhs));
}

PyObjPtr Klass::boolean(const PyObjPtr& obj) {
  auto boolFunc = obj->getattr(PyString::Create("__bool__")->as<PyString>());
  if (boolFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(boolFunc, CreatePyList());
  }
  auto lenFunc = obj->getattr(PyString::Create("__len__")->as<PyString>());
  if (lenFunc != nullptr) {
    auto len = Runtime::Evaluator::InvokeCallable(lenFunc, CreatePyList());
    return len->ne(CreatePyInteger(0ULL));
  }
  return PyBoolean::Create(true);
}

PyObjPtr Klass::getitem(const PyObjPtr& obj, const PyObjPtr& key) {
  return Invoke(obj, PyString::Create("__getitem__"), CreatePyList({key}));
}

PyObjPtr Klass::setitem(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  return Invoke(
    obj, PyString::Create("__setitem__"), CreatePyList({key, value})
  );
}

PyObjPtr Klass::delitem(const PyObjPtr& obj, const PyObjPtr& key) {
  return Invoke(obj, PyString::Create("__delitem__"), CreatePyList({key}));
}

PyObjPtr Klass::contains(const PyObjPtr& obj, const PyObjPtr& key) {
  return Invoke(obj, PyString::Create("__contains__"), CreatePyList({key}));
}

PyObjPtr Klass::len(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__len__"), {});
}

PyObjPtr Klass::getattr(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!isNative) {
    auto attr = GetAttr(obj, PyString::Create("__getattr__")->as<PyString>());
    if (attr != nullptr) {
      return Runtime::Evaluator::InvokeCallable(attr, CreatePyList({key}));
    }
  }
  auto keyStr = key->as<PyString>();
  // attr为nullptr说明没有重载，直接返回属性
  if (obj->Attributes()->Contains(keyStr)) {
    auto attr = obj->Attributes()->Get(keyStr);
    if (attr->is(IifeKlass::Self())) {
      return attr->as<PyIife>()->Call(CreatePyList({obj}));
    }
    return attr;
  }
  if (obj->Methods()->Contains(keyStr)) {
    return BindSelf(obj, obj->Methods()->Get(keyStr));
  }
  // 如果getattr被重载，那么调用重载的函数
  auto attr = GetAttr(obj, PyString::Create("__getattr__")->as<PyString>());
  if (attr != nullptr) {
    return Runtime::Evaluator::InvokeCallable(attr, CreatePyList({key}));
  }
  // 对象属性内部没有找到，查找父类
  attr = GetAttr(obj, keyStr);
  if (attr != nullptr) {
    CacheAttr(obj, keyStr, attr);
    return BindSelf(obj, attr);
  }
  return nullptr;
}

PyObjPtr Klass::setattr(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  if (!isNative) {
    auto attr = GetAttr(obj, PyString::Create("__setattr__")->as<PyString>());
    if (attr != nullptr) {
      return Runtime::Evaluator::InvokeCallable(
        attr, CreatePyList({key, value})
      );
    }
  }
  obj->Attributes()->Put(key->as<PyString>(), value);
  return CreatePyNone();
}

PyObjPtr Klass::str(const PyObjPtr& self) {
  if (isNative) {
    return repr(self);
  }
  auto strFunc = self->getattr(PyString::Create("__str__")->as<PyString>());
  if (strFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(strFunc, CreatePyList());
  }
  auto reprFunc = self->getattr(PyString::Create("__repr__")->as<PyString>());
  if (reprFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(reprFunc, CreatePyList());
  }
  return repr(self);
}

PyObjPtr Klass::matmul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, PyString::Create("__"), CreatePyList({rhs}));
}

PyObjPtr Klass::iter(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__iter__"), CreatePyList());
}

PyObjPtr Klass::next(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__next__"), CreatePyList());
}

PyObjPtr Klass::reversed(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("__reversed__"), CreatePyList());
}

PyObjPtr Klass::_serialize_(const PyObjPtr& obj) {
  return Invoke(obj, PyString::Create("_serialize_"), CreatePyList());
}

void Klass::AddAttribute(const PyStrPtr& key, const PyObjPtr& value) {
  this->attributes->Put(key, value);
}
}  // namespace kaubo::Object