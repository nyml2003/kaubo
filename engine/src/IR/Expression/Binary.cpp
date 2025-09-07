#include "IR/Expression/Binary.h"
#include "ByteCode/ByteCode.h"
#include "Object/Core/PyNone.h"
#include "Object/Runtime/PyInst.h"

namespace kaubo::IR {

Object::PyObjPtr BinaryKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto binary = obj->as<Binary>();
  auto left = binary->Left();
  auto right = binary->Right();
  left->visit(codeList);
  right->visit(codeList);
  return Object::CreatePyNone();
}
Object::PyObjPtr BinaryKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto binary = obj->as<Binary>();
  auto left = binary->Left();
  auto right = binary->Right();
  left->emit(codeList);
  right->emit(codeList);
  Object::PyObjPtr inst = nullptr;
  switch (binary->Oprt()) {
    case Binary::Operator::ADD:
      inst = Object::MakeInst<Object::ByteCode::BINARY_ADD>();
      break;
    case Binary::Operator::SUB:
      inst = Object::MakeInst<Object::ByteCode::BINARY_SUBTRACT>();
      break;
    case Binary::Operator::MUL:
      inst = Object::MakeInst<Object::ByteCode::BINARY_MULTIPLY>();
      break;
    case Binary::Operator::MATMUL:
      inst = Object::MakeInst<Object::ByteCode::BINARY_MATRIX_MULTIPLY>();
      break;
    case Binary::Operator::EQ:
      inst =
        Object::MakeInst<Object::ByteCode::COMPARE_OP>(Object::CompareOp::EQUAL
        );
      break;
    case Binary::Operator::GE:
      inst = Object::MakeInst<Object::ByteCode::COMPARE_OP>(
        Object::CompareOp::GREATER_THAN_EQUAL
      );
      break;
    case Binary::Operator::LE:
      inst = Object::MakeInst<Object::ByteCode::COMPARE_OP>(
        Object::CompareOp::LESS_THAN_EQUAL
      );
      break;
    case Binary::Operator::NE:
      inst = Object::MakeInst<Object::ByteCode::COMPARE_OP>(
        Object::CompareOp::NOT_EQUAL
      );
      break;
    case Binary::Operator::LT:
      inst = Object::MakeInst<Object::ByteCode::COMPARE_OP>(
        Object::CompareOp::LESS_THAN
      );
      break;
    case Binary::Operator::GT:
      inst = Object::MakeInst<Object::ByteCode::COMPARE_OP>(
        Object::CompareOp::GREATER_THAN
      );
      break;
    case Binary::Operator::IS:
      inst =
        Object::MakeInst<Object::ByteCode::COMPARE_OP>(Object::CompareOp::IS);
      break;
    case Binary::Operator::IS_NOT:
      inst =
        Object::MakeInst<Object::ByteCode::COMPARE_OP>(Object::CompareOp::IS_NOT
        );
      break;
    case Binary::Operator::IN_OP:
      inst =
        Object::MakeInst<Object::ByteCode::COMPARE_OP>(Object::CompareOp::IN);
      break;
    case Binary::Operator::NOT_IN:
      inst =
        Object::MakeInst<Object::ByteCode::COMPARE_OP>(Object::CompareOp::NOT_IN
        );
      break;
    case Binary::Operator::SUBSCR:
      inst = Object::MakeInst<Object::ByteCode::BINARY_SUBSCR>();
      break;
    case Binary::Operator::STORE_SUBSCR:
      inst = Object::MakeInst<Object::ByteCode::STORE_SUBSCR>();
      break;
    case Binary::Operator::FLOOR_DIV:
      inst = Object::MakeInst<Object::ByteCode::BINARY_FLOOR_DIVIDE>();
      break;
    case Binary::Operator::DIV:
      inst = Object::MakeInst<Object::ByteCode::BINARY_TRUE_DIVIDE>();
      break;
    case Binary::Operator::MOD:
      inst = Object::MakeInst<Object::ByteCode::BINARY_MODULO>();
      break;
    case Binary::Operator::AND:
      inst = Object::MakeInst<Object::ByteCode::BINARY_AND>();
      break;
    case Binary::Operator::OR:
      inst = Object::MakeInst<Object::ByteCode::BINARY_OR>();
      break;
    case Binary::Operator::XOR:
      inst = Object::MakeInst<Object::ByteCode::BINARY_XOR>();
      break;
    case Binary::Operator::LSHIFT:
      inst = Object::MakeInst<Object::ByteCode::BINARY_LSHIFT>();
      break;
    case Binary::Operator::RSHIFT:
      inst = Object::MakeInst<Object::ByteCode::BINARY_RSHIFT>();
      break;
    case Binary::Operator::POWER:
      inst = Object::MakeInst<Object::ByteCode::BINARY_POWER>();
      break;
  }
  if (inst == nullptr) {
    throw std::runtime_error("BinaryKlass::emit(): unsupported operator");
  }
  auto code = GetCodeFromList(codeList, binary);
  code->Instructions()->Append(inst);
  return Object::CreatePyNone();
}

Object::PyObjPtr BinaryKlass::print(const Object::PyObjPtr& obj) {
  auto binary = obj->as<Binary>();
  auto left = binary->Left();
  auto right = binary->Right();
  auto leftStr = left->print();
  auto rightStr = right->print();
  PrintNode(
    binary, Object::StringConcat(
              Object::CreatePyList({
                Object::CreatePyString("Binary "),
                Stringify(binary->Oprt()),
              })
            )
              ->as<Object::PyString>()
  );
  PrintEdge(binary, left);
  PrintEdge(binary, right);
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR
