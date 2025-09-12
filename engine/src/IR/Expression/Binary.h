#pragma once

#include <cstdint>
#include "IR/INode.h"

namespace kaubo::IR {

class BinaryKlass : public INodeTrait, public Object::KlassBase<BinaryKlass> {
 public:
  explicit BinaryKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_binary"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class Binary : public IR::INode {
 public:
  enum class Operator : uint8_t {
    IN_OP,         // in
    LT,            // <
    GT,            // >
    EQ,            // ==
    GE,            // >=
    LE,            // <=
    NE,            // "!="
    NOT_IN,        // not in
    IS,            // is
    IS_NOT,        // is not
    ADD,           // +
    SUB,           // -
    MUL,           // "*"
    DIV,           // "/"
    MATMUL,        // @
    MOD,           // %
    FLOOR_DIV,     // "//"
    SUBSCR,        // []
    STORE_SUBSCR,  // []=
    AND,           // and
    OR,            // or
    XOR,           // xor
    LSHIFT,        // <<
    RSHIFT,        // >>
    POWER,         // "**"
  };

  explicit Binary(Operator oprt, INodePtr left, INodePtr right, INodePtr parent)
    : INode(BinaryKlass::Self(), std::move(parent)),
      oprt(oprt),
      left(std::move(left)),
      right(std::move(right)) {}

  [[nodiscard]] INodePtr Left() const { return left; }

  [[nodiscard]] INodePtr Right() const { return right; }

  [[nodiscard]] Operator Oprt() const { return oprt; }

  void SetOprt(Operator _oprt) { this->oprt = _oprt; }

 private:
  Operator oprt;
  INodePtr left;
  INodePtr right;
};

inline INodePtr CreateBinary(
  Binary::Operator oprt,
  const INodePtr& left,
  const INodePtr& right,
  const INodePtr& parent
) {
  return std::make_shared<Binary>(oprt, left, right, parent);
}

inline Object::PyStrPtr Stringify(Binary::Operator oprt) {
  switch (oprt) {
    case Binary::Operator::IN_OP:
      return Object::PyString::Create("in");
    case Binary::Operator::LT:
      return Object::PyString::Create("<");
    case Binary::Operator::GT:
      return Object::PyString::Create(">");
    case Binary::Operator::EQ:
      return Object::PyString::Create("==");
    case Binary::Operator::GE:
      return Object::PyString::Create(">=");
    case Binary::Operator::LE:
      return Object::PyString::Create("<=");
    case Binary::Operator::NE:
      return Object::PyString::Create("!=");
    case Binary::Operator::NOT_IN:
      return Object::PyString::Create("not in");
    case Binary::Operator::IS:
      return Object::PyString::Create("is");
    case Binary::Operator::IS_NOT:
      return Object::PyString::Create("is not");
    case Binary::Operator::ADD:
      return Object::PyString::Create("+");
    case Binary::Operator::SUB:
      return Object::PyString::Create("-");
    case Binary::Operator::MUL:
      return Object::PyString::Create("*");
    case Binary::Operator::DIV:
      return Object::PyString::Create("/");
    case Binary::Operator::MATMUL:
      return Object::PyString::Create("@");
    case Binary::Operator::MOD:
      return Object::PyString::Create("%");
    case Binary::Operator::FLOOR_DIV:
      return Object::PyString::Create("//");
    case Binary::Operator::SUBSCR:
      return Object::PyString::Create("[]");
    case Binary::Operator::STORE_SUBSCR:
      return Object::PyString::Create("[]=");
    case Binary::Operator::AND:
      return Object::PyString::Create("and");
    case Binary::Operator::OR:
      return Object::PyString::Create("or");
    case Binary::Operator::XOR:
      return Object::PyString::Create("xor");
    case Binary::Operator::LSHIFT:
      return Object::PyString::Create("<<");
    case Binary::Operator::RSHIFT:
      return Object::PyString::Create(">>");
    case Binary::Operator::POWER:
      return Object::PyString::Create("**");
    default:
      throw std::runtime_error("Invalid binary operator");
  }
}
}  // namespace kaubo::IR
