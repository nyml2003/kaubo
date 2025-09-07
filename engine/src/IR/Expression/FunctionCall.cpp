#include "IR/Expression/FunctionCall.h"
#include "Object/Core/PyNone.h"
#include "Object/Iterator/IteratorHelper.h"
namespace kaubo::IR {

Object::PyObjPtr FunctionCallKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto functionCall = obj->as<FunctionCall>();
  auto func = functionCall->Func();
  auto args = functionCall->Args();
  func->emit(codeList);
  Object::ForEach(args, [&codeList](const Object::PyObjPtr& arg) {
    arg->as<INode>()->emit(codeList);
  });
  auto code = GetCodeFromList(codeList, functionCall);
  code->CallFunction(args->Length());
  return Object::CreatePyNone();
}

Object::PyObjPtr FunctionCallKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto functionCall = obj->as<FunctionCall>();
  auto func = functionCall->Func();
  auto args = functionCall->Args();
  func->visit(codeList);
  Object::ForEach(args, [&codeList](const Object::PyObjPtr& arg) {
    arg->as<INode>()->visit(codeList);
  });
  return Object::CreatePyNone();
}

Object::PyObjPtr FunctionCallKlass::print(const Object::PyObjPtr& obj) {
  auto functionCall = obj->as<FunctionCall>();
  auto func = functionCall->Func();
  auto args = functionCall->Args();
  PrintNode(functionCall, Object::CreatePyString("FunctionCall"));
  func->print();
  PrintEdge(functionCall, func, Object::CreatePyString("function"));
  Object::ForEach(args, [&](const Object::PyObjPtr& arg) {
    arg->as<INode>()->print();
    PrintEdge(functionCall, arg, Object::CreatePyString("argument"));
  });
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR