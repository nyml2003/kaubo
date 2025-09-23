#include "IR/Lambda.h"
#include <memory>
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

Lambda::Lambda(
  Object::PyListPtr parameters,
  Object::PyListPtr body,
  const INodePtr& parent
)
  : INode(LambdaKlass::Self(), parent),
    body(std::move(body)),
    parameters(std::move(parameters)),
    parents(Object::PyList::Create()),
    codeIndex(0) {
  if (parent->is(ModuleKlass::Self())) {
    parents = Object::PyList::Create<Object::PyObjPtr>({parent});
  }
  if (parent->is(LambdaKlass::Self())) {
    parents = parent->as<Lambda>()->Parents();
    parents->Append(parent);
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

auto LambdaKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) -> Object::PyObjPtr {
  auto funcDef = obj->as<Lambda>();
  funcDef->SetCodeIndex(codeList->as<Object::PyList>()->Length());
  auto name = Object::PyString::Create("<lambda>");
  auto code = Object::CreatePyCode(name);
  code->SetScope(Object::Scope::LOCAL);
  code->SetInstructions(Object::PyList::Create());
  Object::ForEach(
    funcDef->Parameters(),
    [&code](const Object::PyObjPtr& param) { code->RegisterVarName(param); }
  );
  code->RegisterConst(Object::PyNone::Create());
  codeList->as<Object::PyList>()->Append(code);
  auto parent = GetCodeFromList(codeList, funcDef->Parent());
  Object::ForEach(funcDef->Body(), [&codeList](const Object::PyObjPtr& stmt) {
    stmt->as<INode>()->visit(codeList);
  });
  parent->RegisterConst(code);
  parent->RegisterConst(name);
  return Object::PyNone::Create();
}

Object::PyObjPtr LambdaKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto funcDef = obj->as<Lambda>();
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
  parent->LoadConst(Object::PyString::Create("<lambda>"));
  parent->MakeFunction();
  if (Config::has("show_bc")) {
    VerboseTerminal::get_instance().switch_strategy(
      std::make_unique<ProxyTerminalStrategy>(&BytecodeTerminal::get_instance())
    );
    Object::PrintCode(selfCode);
  }
  return Object::PyNone::Create();
}

Object::PyObjPtr LambdaKlass::print(const Object::PyObjPtr& obj) {
  auto funcDef = obj->as<Lambda>();
  PrintNode(
    funcDef, Object::StringConcat(
               Object::PyList::Create<Object::PyObjPtr>(
                 {Object::PyString::Create("Lambda ")}
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