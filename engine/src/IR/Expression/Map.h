#pragma once

#include <utility>

#include "IR/INode.h"

namespace kaubo::IR {

class MapKlass : public INodeTrait, public Object::KlassBase<MapKlass> {
 public:
  MapKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_map"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class Map : public INode {
 public:
  explicit Map(
    Object::PyListPtr keys,
    Object::PyListPtr values,
    const INodePtr& parent
  )
    : INode(MapKlass::Self(), parent),
      keys(std::move(keys)),
      values(std::move(values)) {}

  Object::PyListPtr Keys() const { return keys; }
  Object::PyListPtr Values() const { return values; }

 private:
  Object::PyListPtr keys;
  Object::PyListPtr values;
};

using MapPtr = std::shared_ptr<Map>;

inline INodePtr CreateMap(
  const Object::PyListPtr& keys,
  const Object::PyListPtr& values,
  const INodePtr& parent
) {
  return std::make_shared<Map>(keys, values, parent);
}

}  // namespace kaubo::IR
