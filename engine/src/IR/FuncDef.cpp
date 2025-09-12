#include "IR/FuncDef.h"
#include <memory>
#include "IR/ClassDef.h"
#include "IR/INode.h"
#include "IR/Module.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Runtime/PyCode.h"
#include "Tools/Config/Config.h"

#include "Tools/Terminal/VerboseTerminal.h"
namespace kaubo::IR {

FuncDef::FuncDef(
  Object::PyStrPtr name,
  Object::PyListPtr parameters,
  Object::PyListPtr body,
  const INodePtr& parent
)
  : INode(FuncDefKlass::Self(), parent),
    name(std::move(name)),
    body(std::move(body)),
    parameters(std::move(parameters)),
    parents(Object::PyList::Create()),
    codeIndex(0) {
  if (parent->is(ModuleKlass::Self())) {
    parents = Object::PyList::Create<Object::PyObjPtr>({parent});
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

Object::PyObjPtr FuncDefKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto funcDef = obj->as<FuncDef>();
  funcDef->SetCodeIndex(codeList->as<Object::PyList>()->Length());
  auto code = Object::CreatePyCode(funcDef->Name());
  code->SetScope(Object::Scope::LOCAL);
  code->SetInstructions(Object::PyList::Create());
  Object::ForEach(
    funcDef->Parameters(),
    [&code](const Object::PyObjPtr& param) { code->RegisterVarName(param); }
  );
  code->RegisterConst(Object::PyNone::Create());
  codeList->as<Object::PyList>()->Append(code);
  auto parent = GetCodeFromList(codeList, funcDef->Parent());
  parent->RegisterName(funcDef->Name());
  Object::ForEach(funcDef->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->visit(codeList);
  });
  parent->RegisterConst(code);
  parent->RegisterConst(funcDef->Name());
  return Object::PyNone::Create();
}

Object::PyObjPtr FuncDefKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto funcDef = obj->as<FuncDef>();
  auto selfCode = GetCodeFromList(codeList, funcDef);
  Object::ForEach(funcDef->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->emit(codeList);
  });
  if (selfCode->VarNames()->len()->ge(funcDef->Parameters()->len())) {
    selfCode->SetNLocals(selfCode->VarNames()->Length());
  } else {
    selfCode->SetNLocals(funcDef->Parameters()->Length());
  }
  selfCode->LoadConst(Object::PyNone::Create());
  selfCode->ReturnValue();
  auto parent = GetCodeFromList(codeList, funcDef->Parent());
  parent->LoadConst(selfCode);
  parent->LoadConst(funcDef->Name());
  parent->MakeFunction();
  parent->StoreName(funcDef->Name());
  if (Config::has("show_bc")) {
    VerboseTerminal::get_instance().switch_strategy(
      std::make_unique<ProxyTerminalStrategy>(&BytecodeTerminal::get_instance())
    );
    Object::PrintCode(selfCode);
  }
  return Object::PyNone::Create();
}

Object::PyObjPtr FuncDefKlass::print(const Object::PyObjPtr& obj) {
  auto funcDef = obj->as<FuncDef>();
  PrintNode(
    funcDef, Object::StringConcat(
               Object::PyList::Create<Object::PyObjPtr>(
                 {Object::PyString::Create("FuncDef "), funcDef->Name()}
               )
             )
               ->as<Object::PyString>()
  );
  Object::ForEach(funcDef->Body(), [&funcDef](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->print();
    PrintEdge(funcDef, stmt);
  });
  return Object::PyNone::Create();
}
}  // namespace kaubo::IR