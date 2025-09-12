#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class SliceKlass : public INodeTrait, public Object::KlassBase<SliceKlass> {
 public:
  SliceKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_slice"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

// 作为右值的list
class Slice : public INode {
 public:
  explicit Slice(Object::PyListPtr elements, INodePtr parent)
    : INode(SliceKlass::Self(), std::move(parent)),
      elements(std::move(elements)) {}

  [[nodiscard]] Object::PyListPtr Elements() const { return elements; }

 private:
  Object::PyListPtr elements;
};

using SlicePtr = std::shared_ptr<Slice>;

inline INodePtr
CreateSlice(const Object::PyListPtr& elements, const INodePtr& parent) {
  return std::make_shared<Slice>(elements, parent);
}

}  // namespace kaubo::IR
