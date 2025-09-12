#include "IR/ClassDef.h"
#include "IR/FuncDef.h"
#include "IR/INode.h"
#include "IR/Module.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Runtime/PyCode.h"
#include "Object/String/PyString.h"
#include "Tools/Config/Config.h"

#include "Tools/Terminal/VerboseTerminal.h"
namespace kaubo::IR {

ClassDef::ClassDef(
  Object::PyStrPtr name,
  INodePtr bases,
  const INodePtr& parent
)
  : INode(ClassDefKlass::Self(), parent),
    name(std::move(name)),
    body(Object::CreatePyList()),
    bases(std::move(bases)),
    codeIndex(0) {
  if (parent->is(ModuleKlass::Self())) {
    parents = Object::CreatePyList({parent});
  }
  if (parent->is(FuncDefKlass::Self())) {
    parents = parent->as<FuncDef>()->Parents();
    parents->Append(parent);
  }
  if (parent->is(ClassDefKlass::Self())) {
    parents = parent->as<ClassDef>()->Parents();
    parents->Append(parent);
  }
}

Object::PyObjPtr ClassDefKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto classDef = obj->as<ClassDef>();
  classDef->SetCodeIndex(codeList->as<Object::PyList>()->Length());
  auto code = Object::CreatePyCode(classDef->Name());
  code->RegisterName(Object::PyString::Create("__name__"));
  code->RegisterName(Object::PyString::Create("__module__"));
  code->RegisterName(Object::PyString::Create("__qualname__"));
  code->RegisterConst(classDef->Name());
  code->SetScope(Object::Scope::GLOBAL);
  code->SetInstructions(Object::CreatePyList());
  codeList->as<Object::PyList>()->Append(code);
  classDef->Bases()->visit(codeList);
  auto parent = GetCodeFromList(codeList, classDef->Parent());
  parent->RegisterName(classDef->Name());
  Object::ForEach(classDef->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->visit(codeList);
  });
  code->RegisterConst(Object::CreatePyNone());
  parent->RegisterConst(code);
  parent->RegisterConst(classDef->Name());
  return Object::CreatePyNone();
}

Object::PyObjPtr ClassDefKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto classDef = obj->as<ClassDef>();
  auto selfCode = GetCodeFromList(codeList, classDef);
  selfCode->LoadName(Object::PyString::Create("__name__"));
  selfCode->StoreName(Object::PyString::Create("__module__"));
  selfCode->LoadConst(classDef->Name());
  selfCode->StoreName(Object::PyString::Create("__qualname__"));
  Object::ForEach(classDef->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->emit(codeList);
  });
  selfCode->LoadConst(Object::CreatePyNone());
  selfCode->ReturnValue();
  auto parent = GetCodeFromList(codeList, classDef->Parent());
  parent->LoadBuildClass();
  parent->LoadConst(selfCode);
  parent->LoadConst(classDef->Name());
  parent->MakeFunction();
  parent->LoadConst(classDef->Name());
  classDef->Bases()->emit(codeList);
  parent->CallFunction(3);
  parent->StoreName(classDef->Name());
  if (Config::has("show_bc")) {
    VerboseTerminal::get_instance().switch_strategy(
      std::make_unique<ProxyTerminalStrategy>(&BytecodeTerminal::get_instance())
    );
    Object::PrintCode(selfCode);
  }
  return Object::CreatePyNone();
}

Object::PyObjPtr ClassDefKlass::print(const Object::PyObjPtr& obj) {
  auto classDef = obj->as<ClassDef>();
  PrintNode(
    classDef, Object::StringConcat(
                Object::CreatePyList(
                  {Object::PyString::Create("ClassDef "), classDef->Name()}
                )
              )
                ->as<Object::PyString>()
  );
  classDef->Bases()->print();
  PrintEdge(classDef, classDef->Bases());
  Object::ForEach(classDef->Body(), [&classDef](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->print();
    PrintEdge(classDef, stmt);
  });
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR