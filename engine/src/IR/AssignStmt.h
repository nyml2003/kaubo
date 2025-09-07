#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class AssignStmtKlass : public INodeTrait,
                        public Object::KlassBase<AssignStmtKlass> {
 public:
  explicit AssignStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_assignstmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class AssignStmt : public INode {
 public:
  explicit AssignStmt(INodePtr target, INodePtr source, const INodePtr& parent)
    : INode(AssignStmtKlass::Self(), parent),
      target(std::move(target)),
      source(std::move(source)) {}

  INodePtr Target() const { return target; }

  INodePtr Source() const { return source; }

 private:
  INodePtr target;
  INodePtr source;
};

inline INodePtr
CreateAssignStmt(INodePtr target, INodePtr source, INodePtr parent) {
  return std::make_shared<AssignStmt>(
    std::move(target), std::move(source), std::move(parent)
  );
}

}  // namespace kaubo::IR
