#include "IR/Expression/YieldExpr.h"
#include "Object/Core/PyNone.h"

namespace kaubo::IR {

Object::PyObjPtr YieldExprKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<YieldExpr>();
  auto content = stmt->Content();
  content->visit(codeList);
  return Object::CreatePyNone();
}

Object::PyObjPtr YieldExprKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto stmt = obj->as<YieldExpr>();
  auto content = stmt->Content();
  content->emit(codeList);
  auto code = GetCodeFromList(codeList, stmt);
  code->EnableGenerator();
  code->YieldValue();
  return Object::CreatePyNone();
}

Object::PyObjPtr YieldExprKlass::print(const Object::PyObjPtr& obj) {
  auto stmt = obj->as<YieldExpr>();
  auto content = stmt->Content();
  PrintNode(stmt, Object::CreatePyString("YieldExpr"));
  content->print();
  PrintEdge(stmt, content);
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR
