#pragma once
#include "IR/INode.h"
#include "Object/Object.h"

namespace kaubo::IR {

class IfStmtKlass : public INodeTrait, public Object::KlassBase<IfStmtKlass> {
 public:
  explicit IfStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_ifstmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class IfStmt : public INode {
 public:
  explicit IfStmt(
    INodePtr condition,
    Object::PyListPtr thenStmts,
    Object::PyListPtr elseStmts,
    Object::PyListPtr elifs,
    Object::PyListPtr elifConditions,
    INodePtr parent
  )
    : INode(IfStmtKlass::Self(), std::move(parent)),
      condition(std::move(condition)),
      thenStmts(std::move(thenStmts)),
      elseStmts(std::move(elseStmts)),
      elifs(std::move(elifs)),
      elifConditions(std::move(elifConditions)) {}

  [[nodiscard]] INodePtr Condition() const { return condition; }

  [[nodiscard]] Object::PyListPtr ThenStmts() const { return thenStmts; }

  [[nodiscard]] Object::PyListPtr ElseStmts() const { return elseStmts; }

  [[nodiscard]] Object::PyListPtr Elifs() const { return elifs; }

  [[nodiscard]] Object::PyListPtr ElifConditions() const {
    return elifConditions;
  }

 private:
  INodePtr condition;
  Object::PyListPtr thenStmts;
  Object::PyListPtr elseStmts;
  Object::PyListPtr elifs;           // List<List<Stmt>>
  Object::PyListPtr elifConditions;  // List<INodePtr>
};

inline INodePtr CreateIfStmt(
  const INodePtr& condition,
  const Object::PyListPtr& thenStmts,
  const Object::PyListPtr& elseStmts,
  const Object::PyListPtr& elifs,
  const Object::PyListPtr& elifConditions,
  const INodePtr& parent
) {
  return std::make_shared<IfStmt>(
    condition, thenStmts, elseStmts, elifs, elifConditions, parent
  );
}

}  // namespace kaubo::IR
