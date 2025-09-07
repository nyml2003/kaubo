#pragma once

#include <utility>

#include "IR/INode.h"
#include "Object/Core/Klass.h"

namespace kaubo::IR {

class ListKlass : public INodeTrait, public Object::KlassBase<ListKlass> {
 public:
  ListKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_list"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

// 作为右值的list
class List : public INode {
 public:
  explicit List(Object::PyListPtr elements, INodePtr parent)
    : INode(ListKlass::Self(), std::move(parent)),
      elements(std::move(elements)) {}

  [[nodiscard]] Object::PyListPtr Elements() const { return elements; }

 private:
  Object::PyListPtr elements;
};

using ListPtr = std::shared_ptr<List>;

inline INodePtr
CreateList(const Object::PyListPtr& elements, const INodePtr& parent) {
  return std::make_shared<List>(elements, parent);
}

}  // namespace kaubo::IR
