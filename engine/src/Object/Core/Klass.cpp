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
  Invoke(instance, CreatePyString("__init__"), args->as<PyList>());
  return instance;
}

PyObjPtr Klass::add(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__add__"), CreatePyList({rhs}));
}

PyObjPtr Klass::sub(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__sub__"), CreatePyList({rhs}));
}

PyObjPtr Klass::mul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__mul__"), CreatePyList({rhs}));
}

PyObjPtr Klass::floordiv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__floordiv__"), CreatePyList({rhs}));
}

PyObjPtr Klass::truediv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__truediv__"), CreatePyList({rhs}));
}

PyObjPtr Klass::pos(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__pos__"), {});
}

PyObjPtr Klass::neg(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__neg__"), {});
}

PyObjPtr Klass::invert(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__invert__"), {});
}

PyObjPtr Klass::_and_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__and__"), CreatePyList({rhs}));
}

PyObjPtr Klass::_or_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__or__"), CreatePyList({rhs}));
}

PyObjPtr Klass::_xor_(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__xor__"), CreatePyList({rhs}));
}

PyObjPtr Klass::lshift(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__lshift__"), CreatePyList({rhs}));
}

PyObjPtr Klass::rshift(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__rshift__"), CreatePyList({rhs}));
}

PyObjPtr Klass::mod(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__mod__"), CreatePyList({rhs}));
}

PyObjPtr Klass::divmod(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__divmod__"), CreatePyList({rhs}));
}

PyObjPtr Klass::pow(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__pow__"), CreatePyList({rhs}));
}

PyObjPtr Klass::repr(const PyObjPtr& self) {
  if (isNative) {
    return StringConcat(CreatePyList(
      {CreatePyString("<"),
       self->getattr(CreatePyString("__name__")->as<PyString>()),
       CreatePyString(" object at "), Function::Identity(CreatePyList({self})),
       CreatePyString(">")}
    ));
  }
  auto reprFunc = GetAttr(self, CreatePyString("__repr__")->as<PyString>());
  if (reprFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(reprFunc, CreatePyList({self}));
  }
  return StringConcat(CreatePyList(
    {CreatePyString("<"),
     self->getattr(CreatePyString("__name__")->as<PyString>()),
     CreatePyString(" object at "), Function::Identity(CreatePyList({self})),
     CreatePyString(">")}
  ));
}

PyObjPtr Klass::hash(const PyObjPtr& obj) {
  if (obj->Hashed()) {
    return CreatePyInteger(obj->HashValue());
  }
  auto hashFunc = obj->getattr(CreatePyString("__hash__")->as<PyString>());
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
  return Invoke(lhs, CreatePyString("__eq__"), CreatePyList({rhs}));
}

PyObjPtr Klass::lt(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__lt__"), CreatePyList({rhs}));
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
  auto boolFunc = obj->getattr(CreatePyString("__bool__")->as<PyString>());
  if (boolFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(boolFunc, CreatePyList());
  }
  auto lenFunc = obj->getattr(CreatePyString("__len__")->as<PyString>());
  if (lenFunc != nullptr) {
    auto len = Runtime::Evaluator::InvokeCallable(lenFunc, CreatePyList());
    return len->ne(CreatePyInteger(0ULL));
  }
  return CreatePyBoolean(true);
}

PyObjPtr Klass::getitem(const PyObjPtr& obj, const PyObjPtr& key) {
  return Invoke(obj, CreatePyString("__getitem__"), CreatePyList({key}));
}

PyObjPtr Klass::setitem(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  return Invoke(obj, CreatePyString("__setitem__"), CreatePyList({key, value}));
}

PyObjPtr Klass::delitem(const PyObjPtr& obj, const PyObjPtr& key) {
  return Invoke(obj, CreatePyString("__delitem__"), CreatePyList({key}));
}

PyObjPtr Klass::contains(const PyObjPtr& obj, const PyObjPtr& key) {
  return Invoke(obj, CreatePyString("__contains__"), CreatePyList({key}));
}

PyObjPtr Klass::len(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__len__"), {});
}

PyObjPtr Klass::getattr(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!isNative) {
    auto attr = GetAttr(obj, CreatePyString("__getattr__")->as<PyString>());
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
  auto attr = GetAttr(obj, CreatePyString("__getattr__")->as<PyString>());
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
    auto attr = GetAttr(obj, CreatePyString("__setattr__")->as<PyString>());
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
  auto strFunc = self->getattr(CreatePyString("__str__")->as<PyString>());
  if (strFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(strFunc, CreatePyList());
  }
  auto reprFunc = self->getattr(CreatePyString("__repr__")->as<PyString>());
  if (reprFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(reprFunc, CreatePyList());
  }
  return repr(self);
}

PyObjPtr Klass::matmul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return Invoke(lhs, CreatePyString("__"), CreatePyList({rhs}));
}

PyObjPtr Klass::iter(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__iter__"), CreatePyList());
}

PyObjPtr Klass::next(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__next__"), CreatePyList());
}

PyObjPtr Klass::reversed(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("__reversed__"), CreatePyList());
}

PyObjPtr Klass::_serialize_(const PyObjPtr& obj) {
  return Invoke(obj, CreatePyString("_serialize_"), CreatePyList());
}

void Klass::AddAttribute(const PyStrPtr& key, const PyObjPtr& value) {
  this->attributes->Put(key, value);
}
}  // namespace kaubo::Object