#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class ExprStmtKlass : public INodeTrait,
                      public Object::KlassBase<ExprStmtKlass> {
 public:
  explicit ExprStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_exprstmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class ExprStmt : public INode {
 public:
  explicit ExprStmt(INodePtr content, INodePtr parent)
    : INode(ExprStmtKlass::Self(), std::move(parent)),
      content(std::move(content)) {}

  [[nodiscard]] INodePtr Content() const { return content; }

 private:
  INodePtr content;
};

inline INodePtr
CreateExprStmt(const INodePtr& content, const INodePtr& parent) {
  return std::make_shared<ExprStmt>(content, parent);
}

}  // namespace kaubo::IR
