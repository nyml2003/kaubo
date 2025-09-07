#pragma once

#include "Common.h"
#include "Object/Core/Klass.h"
#include "Object/Object.h"

namespace kaubo::Object {

class PyObject : public std::enable_shared_from_this<PyObject> {
 private:
  KlassPtr klass;
  PyDictPtr attributes;  // 不需要bound的属性
  PyDictPtr methods;     // 需要bound的属性
  Index hashValue{};
  bool hashed = false;
  bool isMarked = false;  // 垃圾回收标记位

 public:
  explicit PyObject(KlassPtr klass) : klass(klass) {}

  // 垃圾回收支持
  bool IsMarked() const { return isMarked; }
  void SetMarked(bool marked) { isMarked = marked; }

  // virtual void Mark() = 0;

  // virtual void Create() = 0;

  [[nodiscard]] KlassPtr Klass() const { return klass; }
  [[nodiscard]] PyDictPtr Attributes() noexcept;
  [[nodiscard]] PyDictPtr Methods() noexcept;
  void SetAttributes(const PyDictPtr& _attributes) { attributes = _attributes; }
  void SetKlass(const KlassPtr& _klass) { klass = _klass; }
  virtual ~PyObject() = default;
  PyObject(const PyObject&) = default;
  PyObject& operator=(const PyObject&) = default;
  PyObject(PyObject&&) = default;
  PyObject& operator=(PyObject&&) = default;
  void SetHashValue(Index value) {
    hashValue = value;
    hashed = true;
  }
  [[nodiscard]] Index HashValue() const { return hashValue; }
  [[nodiscard]] bool Hashed() const { return hashed; }
  PyObjPtr add(const PyObjPtr& other) {
    return klass->add(shared_from_this(), other);
  }
  PyObjPtr sub(const PyObjPtr& other) {
    return klass->sub(shared_from_this(), other);
  }
  PyObjPtr mul(const PyObjPtr& other) {
    return klass->mul(shared_from_this(), other);
  }
  PyObjPtr floordiv(const PyObjPtr& other) {
    return klass->floordiv(shared_from_this(), other);
  }
  PyObjPtr mod(const PyObjPtr& other) {
    return klass->mod(shared_from_this(), other);
  }
  PyObjPtr truediv(const PyObjPtr& other) {
    return klass->truediv(shared_from_this(), other);
  }
  PyObjPtr matmul(const PyObjPtr& other) {
    return klass->matmul(shared_from_this(), other);
  }
  PyObjPtr pos() { return klass->pos(shared_from_this()); }
  PyObjPtr neg() { return klass->neg(shared_from_this()); }
  PyObjPtr invert() { return klass->invert(shared_from_this()); }
  PyObjPtr _and_(const PyObjPtr& other) {
    return klass->_and_(shared_from_this(), other);
  }
  PyObjPtr _or_(const PyObjPtr& other) {
    return klass->_or_(shared_from_this(), other);
  }
  PyObjPtr _xor_(const PyObjPtr& other) {
    return klass->_xor_(shared_from_this(), other);
  }
  PyObjPtr lshift(const PyObjPtr& other) {
    return klass->lshift(shared_from_this(), other);
  }
  PyObjPtr rshift(const PyObjPtr& other) {
    return klass->rshift(shared_from_this(), other);
  }
  PyObjPtr pow(const PyObjPtr& other) {
    return klass->pow(shared_from_this(), other);
  }
  PyObjPtr gt(const PyObjPtr& other) {
    return klass->gt(shared_from_this(), other);
  }
  PyObjPtr lt(const PyObjPtr& other) {
    return klass->lt(shared_from_this(), other);
  }
  PyObjPtr eq(const PyObjPtr& other) {
    return klass->eq(shared_from_this(), other);
  }
  PyObjPtr ge(const PyObjPtr& other) {
    return klass->ge(shared_from_this(), other);
  }
  PyObjPtr le(const PyObjPtr& other) {
    return klass->le(shared_from_this(), other);
  }
  PyObjPtr ne(const PyObjPtr& other) {
    return klass->ne(shared_from_this(), other);
  }
  PyObjPtr hash() { return klass->hash(shared_from_this()); }
  PyObjPtr repr() { return klass->repr(shared_from_this()); }
  PyObjPtr str() { return klass->str(shared_from_this()); }
  PyObjPtr getitem(const PyObjPtr& key) {
    return klass->getitem(shared_from_this(), key);
  }
  PyObjPtr setitem(const PyObjPtr& key, const PyObjPtr& value) {
    return klass->setitem(shared_from_this(), key, value);
  }
  PyObjPtr delitem(const PyObjPtr& key) {
    return klass->delitem(shared_from_this(), key);
  }
  PyObjPtr contains(const PyObjPtr& key) {
    return klass->contains(shared_from_this(), key);
  }
  PyObjPtr len() { return klass->len(shared_from_this()); }
  PyObjPtr boolean() { return klass->boolean(shared_from_this()); }
  PyObjPtr getattr(const PyObjPtr& key) {
    return klass->getattr(shared_from_this(), key);
  }
  PyObjPtr setattr(const PyObjPtr& key, const PyObjPtr& value) {
    return klass->setattr(shared_from_this(), key, value);
  }
  PyObjPtr iter() { return klass->iter(shared_from_this()); }
  PyObjPtr next() { return klass->next(shared_from_this()); }
  PyObjPtr reversed() { return klass->reversed(shared_from_this()); }
  PyObjPtr _serialize_() { return klass->_serialize_(shared_from_this()); }
  bool is(const KlassPtr& _klass) { return klass == _klass; }

  template <typename T>
  std::shared_ptr<T> as() {
    return std::dynamic_pointer_cast<T>(shared_from_this());
  }
};

using PyObjPtr = std::shared_ptr<PyObject>;

class ObjectKlass : public KlassBase<ObjectKlass> {
 public:
  explicit ObjectKlass() = default;
  void Initialize() override;
};

bool operator==(const PyObjPtr& lhs, const PyObjPtr& rhs);

bool operator<(const PyObjPtr& lhs, const PyObjPtr& rhs);

bool operator!=(const PyObjPtr& lhs, const PyObjPtr& rhs);

PyObjPtr ObjectInit(const PyObjPtr& args);

}  // namespace kaubo::Object

