#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class FunctionCallKlass : public INodeTrait,
                          public Object::KlassBase<FunctionCallKlass> {
 public:
  FunctionCallKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_functioncall"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class FunctionCall : public INode {
 public:
  FunctionCall(INodePtr func, Object::PyListPtr args, INodePtr parent)
    : INode(FunctionCallKlass::Self(), std::move(parent)),
      func(std::move(func)),
      args(std::move(args)) {}

  [[nodiscard]] INodePtr Func() const { return func; }

  [[nodiscard]] Object::PyListPtr Args() const { return args; }

 private:
  INodePtr func;
  Object::PyListPtr args;
};

using FunctionCallPtr = std::shared_ptr<FunctionCall>;

inline INodePtr CreateFunctionCall(
  const INodePtr& func,
  const Object::PyListPtr& args,
  const INodePtr& parent
) {
  return std::make_shared<FunctionCall>(func, args, parent);
}

}  // namespace kaubo::IR
