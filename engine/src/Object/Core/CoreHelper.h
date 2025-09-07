#pragma once

#include "Object/String/PyString.h"

#include <optional>
namespace kaubo::Object {

void LoadClass(const Object::PyStrPtr& name, const Object::KlassPtr& klass);
void ConfigureBasicAttributes(const Object::KlassPtr& klass);
void InitKlass(const Object::PyStrPtr& name, const Object::KlassPtr& klass);
Object::PyObjPtr Invoke(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& methodName,
  const Object::PyListPtr& args
);
Object::PyObjPtr
GetAttr(const Object::PyObjPtr& obj, const Object::PyStrPtr& attrName) noexcept;
Object::PyObjPtr
BindSelf(const Object::PyObjPtr& obj, const Object::PyObjPtr& attr);
void CacheAttr(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& key,
  const Object::PyObjPtr& attr
);
Object::PyObjPtr GetBases(const Object::PyObjPtr& args);
Object::PyObjPtr GetMro(const Object::PyObjPtr& args);
Object::PyObjPtr GetDict(const Object::PyObjPtr& args);
// 内置函数str
Object::PyObjPtr Str(const Object::PyObjPtr& args);
// object.__str__
Object::PyObjPtr KlassStr(const Object::PyObjPtr& args);
// 内置函数repr
Object::PyObjPtr Repr(const Object::PyObjPtr& args);
// object.__repr__
Object::PyObjPtr KlassRepr(const Object::PyObjPtr& args);
// 内置函数bool
Object::PyObjPtr Bool(const Object::PyObjPtr& args);
// object.__bool__
Object::PyObjPtr KlassBool(const Object::PyObjPtr& args);

void LoadBootstrapClasses();
void LoadRuntimeSupportClasses();
Object::PyListPtr MergeMro(const Object::PyListPtr& mros);
Object::PyListPtr ComputeMro(const Object::PyTypePtr& type);
bool FirstOrNotInMro(
  const Object::PyListPtr& mro,
  const Object::PyTypePtr& type
);
bool CouldTypePlaceAhead(
  const Object::PyListPtr& mros,
  const Object::PyTypePtr& type,
  Index ignore
);
void RemoveHeadFromMros(
  const PyListPtr& mros,
  Index excludeIndex,
  const PyTypePtr& head
);
void CleanMros(const Object::PyListPtr& mros);
std::optional<std::tuple<Index, PyTypePtr>> FindCandidateBase(
  const PyListPtr& mros
);
Object::KlassPtr CreatePyKlass(
  const Object::PyStrPtr& name,
  const Object::PyDictPtr& attributes,
  const Object::PyListPtr& super
);
}  // namespace kaubo::Object

