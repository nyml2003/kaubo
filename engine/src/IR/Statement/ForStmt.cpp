#include "IR/Statement/ForStmt.h"
#include "ByteCode/ByteCode.h"
#include "Function/BuiltinFunction.h"
#include "IR/INode.h"
#include "IR/Identifier.h"
#include "Object/Core/PyNone.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Runtime/PyInst.h"
namespace kaubo::IR {

Object::PyObjPtr ForStmtKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<ForStmt>();
  auto iter = stmt->Iter();
  iter->emit(codeList);
  auto code = GetCodeFromList(codeList, stmt);
  code->GetIter();
  Index start = code->ForIter(0);
  auto target = stmt->Target();
  if (target->is(IdentifierKlass::Self())) {
    auto identifier = target->as<Identifier>();
    auto name = identifier->Name();
    if (code->GetScope() == Object::Scope::GLOBAL) {
      code->StoreName(name);
    }
    if (code->GetScope() == Object::Scope::LOCAL) {
      code->StoreFast(name);
    }
  } else {
    throw std::runtime_error("ForStmt::emit(): unsupported target type");
  }
  auto body = stmt->Body();
  Object::ForEach(body, [codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->emit(codeList);
  });
  code->JumpAbsolute(start - 1);
  Index end = code->Instructions()->Length();
  code->Instructions()->SetItem(
    start - 1, Object::MakeInst<Object::ByteCode::FOR_ITER>(end - start + 1)
  );
  return Object::PyNone::Create();
}

Object::PyObjPtr ForStmtKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<ForStmt>();
  auto target = stmt->Target();
  auto iter = stmt->Iter();
  auto body = stmt->Body();
  iter->visit(codeList);
  if (target->is(IdentifierKlass::Self())) {
    auto identifier = target->as<Identifier>();
    auto code = GetCodeFromList(codeList, stmt);
    auto name = identifier->Name();
    if (code->GetScope() == Object::Scope::GLOBAL) {
      code->RegisterName(name);
    }
    if (code->GetScope() == Object::Scope::LOCAL) {
      code->RegisterVarName(name);
    }
  } else {
    Function::DebugPrint(target);
    throw std::runtime_error("ForStmt::visit(): unsupported target type");
  }
  Object::ForEach(body, [codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->visit(codeList);
  });
  return Object::PyNone::Create();
}

Object::PyObjPtr ForStmtKlass::print(const Object::PyObjPtr& obj) {
  auto forStmt = obj->as<ForStmt>();
  auto iter = forStmt->Iter();
  auto target = forStmt->Target();
  auto body = forStmt->Body();
  PrintNode(forStmt, Object::PyString::Create("ForStmt"));
  iter->print();
  PrintEdge(forStmt, iter, Object::PyString::Create("iter"));
  target->print();
  PrintEdge(forStmt, target, Object::PyString::Create("target"));
  Object::ForEach(body, [&forStmt](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->print();
    PrintEdge(forStmt, stmt, Object::PyString::Create("stmt"));
  });
  return Object::PyNone::Create();
}

}  // namespace kaubo::IR
