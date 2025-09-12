

#include "IR/INode.h"

namespace kaubo::IR {

class WhileStmtKlass : public INodeTrait,
                       public Object::KlassBase<WhileStmtKlass> {
 public:
  explicit WhileStmtKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_whilestmt"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class WhileStmt : public INode {
 public:
  explicit WhileStmt(
    INodePtr condition,
    Object::PyListPtr body,
    INodePtr parent
  )
    : INode(WhileStmtKlass::Self(), std::move(parent)),
      condition(std::move(condition)),
      body(std::move(body)) {}

  INodePtr Condition() const { return condition; }

  Object::PyListPtr Body() const { return body; }

 private:
  INodePtr condition;
  Object::PyListPtr body;
};

inline INodePtr CreateWhileStmt(
  INodePtr condition,
  const Object::PyListPtr& body,
  INodePtr parent
) {
  return std::make_shared<WhileStmt>(
    std::move(condition), body, std::move(parent)
  );
}

}  // namespace kaubo::IR
