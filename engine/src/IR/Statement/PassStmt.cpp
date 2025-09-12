#include "IR/Statement/PassStmt.h"

namespace kaubo::IR {

Object::PyObjPtr PassStmtKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<PassStmt>();
  auto code = GetCodeFromList(codeList, stmt);
  code->Nop();
  return Object::CreatePyNone();
}

Object::PyObjPtr PassStmtKlass::print(const Object::PyObjPtr& obj) {
  auto stmt = obj->as<PassStmt>();
  PrintNode(stmt, Object::PyString::Create("PassStmt"));
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR