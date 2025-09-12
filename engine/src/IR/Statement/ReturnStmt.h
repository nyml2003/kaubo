#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class ReturnStmtKlass : public INodeTrait,
                        public Object::KlassBase<ReturnStmtKlass> {
 public:
  explicit ReturnStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_returnstmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class ReturnStmt : public INode {
 public:
  explicit ReturnStmt(INodePtr content, INodePtr parent)
    : INode(ReturnStmtKlass::Self(), std::move(parent)),
      content(std::move(content)) {}

  [[nodiscard]] INodePtr Content() const { return content; }

 private:
  INodePtr content;
};

inline INodePtr CreateReturnStmt(INodePtr content, INodePtr parent) {
  return std::make_shared<ReturnStmt>(std::move(content), std::move(parent));
}

}  // namespace kaubo::IR
