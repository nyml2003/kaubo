#pragma once

#include <cstdint>
#include "IR/INode.h"

namespace kaubo::IR {

class UnaryKlass : public INodeTrait, public Object::KlassBase<UnaryKlass> {
 public:
  explicit UnaryKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_unary"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class Unary : public IR::INode {
 public:
  enum class Operator : uint8_t {
    PLUS,   // +
    MINUS,  // -
    NOT,    // not
    INVERT  // ~
  };

  explicit Unary(Operator oprt, INodePtr operand, INodePtr parent)
    : INode(UnaryKlass::Self(), std::move(parent)),
      oprt(oprt),
      operand(std::move(operand)) {}

  [[nodiscard]] INodePtr Operand() const { return operand; }

  [[nodiscard]] Operator Oprt() const { return oprt; }

 private:
  Operator oprt;
  INodePtr operand;
};

inline INodePtr
CreateUnary(Unary::Operator oprt, INodePtr operand, INodePtr parent) {
  return std::make_shared<Unary>(oprt, std::move(operand), std::move(parent));
}

}  // namespace kaubo::IR
