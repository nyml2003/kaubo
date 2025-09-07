#pragma once

#include "IR/INode.h"
#include "Object/Core/PyNone.h"

namespace kaubo::IR {

class PassStmtKlass : public INodeTrait,
                      public Object::KlassBase<PassStmtKlass> {
 public:
  explicit PassStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_passstmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr visit(
    const Object::PyObjPtr& /*obj*/,
    const Object::PyObjPtr& /*codeList*/
  ) override {
    return Object::CreatePyNone();
  }

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class PassStmt : public INode {
 public:
  explicit PassStmt(const INodePtr& parent)
    : INode(PassStmtKlass::Self(), parent) {}
};

inline INodePtr CreatePassStmt(const INodePtr& parent) {
  return std::make_shared<PassStmt>(parent);
}

}  // namespace kaubo::IR
