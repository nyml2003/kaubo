#pragma once
#include "IR/INode.h"

namespace kaubo::IR {

class ForStmtKlass : public INodeTrait, public Object::KlassBase<ForStmtKlass> {
 public:
  explicit ForStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_forstmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class ForStmt : public INode {
 public:
  explicit ForStmt(
    INodePtr target,
    INodePtr iter,
    Object::PyListPtr body,
    INodePtr parent
  )
    : INode(ForStmtKlass::Self(), std::move(parent)),
      target(std::move(target)),
      iter(std::move(iter)),
      body(std::move(body)) {}

  INodePtr Target() const { return target; }

  INodePtr Iter() const { return iter; }

  Object::PyListPtr Body() const { return body; }

 private:
  INodePtr target;
  INodePtr iter;
  Object::PyListPtr body;
};

inline INodePtr CreateForStmt(
  const INodePtr& target,
  const INodePtr& iter,
  const Object::PyListPtr& body,
  const INodePtr& parent
) {
  return std::make_shared<ForStmt>(target, iter, body, parent);
}

}  // namespace kaubo::IR
