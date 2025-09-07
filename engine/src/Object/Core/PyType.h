#pragma once

#include "Object/Core/PyObject.h"

namespace kaubo::Object {

class TypeKlass : public KlassBase<TypeKlass> {
 public:
  explicit TypeKlass() = default;

  void Initialize() override;

  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr repr(const PyObjPtr& obj) override;

  PyObjPtr str(const PyObjPtr& obj) override { return repr(obj); }
};

class PyType : public PyObject {
 private:
  KlassPtr owner;

 public:
  explicit PyType(KlassPtr _owner);

  [[nodiscard]] KlassPtr Owner() const { return owner; }
};

using PyTypePtr = std::shared_ptr<PyType>;

inline PyObjPtr CreatePyType(KlassPtr owner) {
  auto type = std::make_shared<PyType>(owner)->as<PyType>();
  return type;
}
}  // namespace kaubo::Object
