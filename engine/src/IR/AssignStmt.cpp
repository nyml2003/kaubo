#include "IR/AssignStmt.h"
#include "IR/Expression/Binary.h"
#include "IR/Identifier.h"
#include "IR/MemberAccess.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"

namespace kaubo::IR {

Object::PyObjPtr AssignStmtKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto assignStmt = obj->as<AssignStmt>();
  auto source = assignStmt->Source();
  source->emit(codeList);
  auto target = assignStmt->Target();
  if (target->is(IdentifierKlass::Self())) {
    auto targetIdentifier = target->as<Identifier>();
    targetIdentifier->emit(codeList);
  } else if (target->is(MemberAccessKlass::Self())) {
    auto memberAccess = target->as<MemberAccess>();
    memberAccess->emit(codeList);
  } else if (target->is(BinaryKlass::Self())) {
    auto binary = target->as<Binary>();
    binary->emit(codeList);
    return Object::CreatePyNone();
  }
  return Object::CreatePyNone();
}

Object::PyObjPtr AssignStmtKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto assignStmt = obj->as<AssignStmt>();
  auto source = assignStmt->Source();
  source->visit(codeList);
  auto target = assignStmt->Target();
  if (target->is(IdentifierKlass::Self())) {
    auto targetIdentifier = target->as<Identifier>();
    targetIdentifier->SetStoreMode();
    targetIdentifier->visit(codeList);
  } else if (target->is(MemberAccessKlass::Self())) {
    auto memberAccess = target->as<MemberAccess>();
    memberAccess->SetStoreMode();
    memberAccess->visit(codeList);
  } else if (target->is(BinaryKlass::Self())) {
    auto binary = target->as<Binary>();
    auto oprt = binary->Oprt();
    if (oprt != Binary::Operator::SUBSCR) {
      throw std::runtime_error(
        "When assigning, only support subscript operation"
      );
    }
    binary->SetOprt(Binary::Operator::STORE_SUBSCR);
    binary->visit(codeList);
    return Object::CreatePyNone();
  }
  return Object::CreatePyNone();
}

Object::PyObjPtr AssignStmtKlass::print(const Object::PyObjPtr& obj) {
  auto assignStmt = obj->as<AssignStmt>();
  auto source = assignStmt->Source();
  auto target = assignStmt->Target();
  PrintNode(assignStmt, Object::CreatePyString("AssignStmt"));
  source->print();
  PrintEdge(assignStmt, source, Object::CreatePyString("source"));
  target->print();
  PrintEdge(assignStmt, target, Object::CreatePyString("target"));
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR
