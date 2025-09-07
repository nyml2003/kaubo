#include "IR/Statement/ExprStmt.h"
#include "Object/Core/PyNone.h"

namespace kaubo::IR {

Object::PyObjPtr ExprStmtKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<ExprStmt>();
  auto content = stmt->Content();
  content->visit(codeList);
  return Object::CreatePyNone();
}

Object::PyObjPtr ExprStmtKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<ExprStmt>();
  auto content = stmt->Content();
  content->emit(codeList);
  auto code = GetCodeFromList(codeList, stmt);
  code->PopTop();
  return Object::CreatePyNone();
}

Object::PyObjPtr ExprStmtKlass::print(const Object::PyObjPtr& obj) {
  auto stmt = obj->as<ExprStmt>();
  auto content = stmt->Content();
  PrintNode(stmt, Object::CreatePyString("ExprStmt"));
  content->print();
  PrintEdge(stmt, content);
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR
