#include "IR/Module.h"

#include "IR/INode.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Runtime/PyCode.h"
namespace kaubo::IR {

Object::PyObjPtr ModuleKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto module = obj->as<Module>();
  module->SetCodeIndex(codeList->as<Object::PyList>()->Length());
  auto code = Object::CreatePyCode(module->Name());
  code->SetScope(Object::Scope::GLOBAL);
  code->SetInstructions(Object::PyList::Create());
  codeList->as<Object::PyList>()->Append(code);
  Object::ForEach(module->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->visit(codeList);
  });
  code->RegisterConst(Object::PyNone::Create());
  return Object::PyNone::Create();
}

Object::PyObjPtr ModuleKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto module = obj->as<Module>();
  Object::ForEach(module->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->emit(codeList);
  });
  auto selfCode = GetCodeFromList(codeList, module);
  selfCode->LoadConst(Object::PyNone::Create());
  selfCode->ReturnValue();
  return Object::PyNone::Create();
}

Object::PyObjPtr ModuleKlass::print(const Object::PyObjPtr& obj) {
  auto module = obj->as<Module>();
  PrintNode(
    module, Object::StringConcat(
              Object::PyList::Create<Object::PyObjPtr>(
                {Object::PyString::Create("Module "), module->Name()}
              )
            )
              ->as<Object::PyString>()
  );
  Object::ForEach(module->Body(), [&module](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->print();
    PrintEdge(module, stmt);
  });
  return Object::PyNone::Create();
}

}  // namespace kaubo::IR