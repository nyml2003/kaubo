#include "IR/Statement/ReturnStmt.h"
#include "Object/Core/PyNone.h"

namespace kaubo::IR {

Object::PyObjPtr ReturnStmtKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<ReturnStmt>();
  auto content = stmt->Content();
  content->visit(codeList);
  return Object::PyNone::Create();
}

Object::PyObjPtr ReturnStmtKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<ReturnStmt>();
  auto content = stmt->Content();
  content->emit(codeList);
  auto code = GetCodeFromList(codeList, stmt);
  code->ReturnValue();
  return Object::PyNone::Create();
}

Object::PyObjPtr ReturnStmtKlass::print(const Object::PyObjPtr& obj) {
  auto stmt = obj->as<ReturnStmt>();
  auto content = stmt->Content();
  PrintNode(stmt, Object::PyString::Create("ReturnStmt"));
  content->print();
  PrintEdge(stmt, content);
  return Object::PyNone::Create();
}
}  // namespace kaubo::IR
