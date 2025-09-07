#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class YieldExprKlass : public INodeTrait,
                       public Object::KlassBase<YieldExprKlass> {
 public:
  explicit YieldExprKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_YieldExpr"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class YieldExpr : public INode {
 public:
  explicit YieldExpr(INodePtr content, INodePtr parent)
    : INode(YieldExprKlass::Self(), std::move(parent)),
      content(std::move(content)) {}

  [[nodiscard]] INodePtr Content() const { return content; }

 private:
  INodePtr content;
};

inline INodePtr CreateYieldExpr(INodePtr content, INodePtr parent) {
  return std::make_shared<YieldExpr>(std::move(content), std::move(parent));
}

}  // namespace kaubo::IR
