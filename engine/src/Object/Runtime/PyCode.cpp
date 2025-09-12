#include "Object/Runtime/PyCode.h"
#include "ByteCode/ByteCode.h"
#include "Collections/String/BytesHelper.h"
#include "Function/BuiltinFunction.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyObject.h"
#include "Object/Object.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"
#include "Tools/Terminal/VerboseTerminal.h"

namespace kaubo::Object {

PyCode::PyCode(
  PyBytesPtr byteCodes,
  PyListPtr consts,
  PyListPtr names,
  PyListPtr varNames,
  PyStrPtr name,
  Index nLocals,
  bool isGenerator
)
  : PyObject(CodeKlass::Self()),
    byteCode(std::move(byteCodes)),
    instructions(nullptr),
    consts(std::move(consts)),
    names(std::move(names)),
    varNames(std::move(varNames)),
    name(std::move(name)),
    nLocals(nLocals),
    isGenerator(isGenerator) {}

PyListPtr PyCode::Instructions() {
  return instructions;
}

void PyCode::SetInstructions(PyListPtr&& insts) {
  instructions = std::move(insts);
}

void PyCode::SetByteCode(const PyBytesPtr& byteCodes) {
  byteCode = byteCodes;
}

void PyCode::SetNLocals(Index _nLocals) {
  this->nLocals = _nLocals;
}

void PyCode::SetScope(enum Scope _scope) {
  this->scope = _scope;
}

Scope PyCode::GetScope() const {
  return scope;
}

PyBytesPtr PyCode::ByteCode() const {
  return byteCode;
}

PyListPtr PyCode::Consts() const {
  return consts;
}

PyListPtr PyCode::Names() const {
  return names;
}

PyStrPtr PyCode::Name() const {
  return name;
}

PyListPtr PyCode::VarNames() const {
  return varNames;
}

Index PyCode::NLocals() const {
  return nLocals;
}

PyObjPtr CodeKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(CodeKlass::Self()) || !rhs->is(CodeKlass::Self())) {
    return PyBoolean::create(false);
  }
  auto lhsc = lhs->as<PyCode>();
  auto rhsc = rhs->as<PyCode>();
  if (!IsTrue(lhsc->Name()->eq(rhsc->Name()))) {
    return PyBoolean::create(false);
  }
  if (!IsTrue(lhsc->Consts()->eq(rhsc->Consts()))) {
    return PyBoolean::create(false);
  }
  if (!IsTrue(lhsc->Names()->eq(rhsc->Names()))) {
    return PyBoolean::create(false);
  }
  if (!IsTrue(lhsc->VarNames()->eq(rhsc->VarNames()))) {
    return PyBoolean::create(false);
  }
  if (lhsc->NLocals() != rhsc->NLocals()) {
    return PyBoolean::create(false);
  }
  if (!IsTrue(lhsc->Instructions()->eq(rhsc->Instructions()))) {
    return PyBoolean::create(false);
  }
  return PyBoolean::create(true);
}

PyObjPtr CodeKlass::repr(const PyObjPtr& self) {
  return StringConcat(CreatePyList(
    {CreatePyString("<code object at ")->as<PyString>(),
     Function::Identity(CreatePyList({self}))->as<PyString>(),
     CreatePyString(">")->as<PyString>()}
  ));
}

PyObjPtr CodeKlass::_serialize_(const PyObjPtr& self) {
  if (!self->is(CodeKlass::Self())) {
    throw std::runtime_error("PyCode::_serialize_(): obj is not a code object");
  }
  auto code = self->as<PyCode>();
  Collections::StringBuilder result(Collections::Serialize(Literal::CODE));
  result.Append(code->Consts()->_serialize_()->as<PyBytes>()->Value());
  result.Append(code->Names()->_serialize_()->as<PyBytes>()->Value());
  result.Append(code->VarNames()->_serialize_()->as<PyBytes>()->Value());
  result.Append(code->Name()->_serialize_()->as<PyBytes>()->Value());
  result.Append(Collections::Serialize(code->NLocals()));
  result.Append(
    Collections::Serialize(
      code->IsGenerator() ? Literal::TRUE_LITERAL : Literal::FALSE_LITERAL
    )
  );
  result.Append(
    code->Instructions()->_serialize_()->_serialize_()->as<PyBytes>()->Value()
  );
  return CreatePyBytes(result.ToString());
}

Index PyCode::IndexOfConst(const PyObjPtr& obj) {
  if (!consts->Contains(obj)) {
    Function::DebugPrint(obj);
    throw std::runtime_error("PyCode::IndexOfConst(): obj not found");
  }
  return consts->IndexOf(obj);
}

void PyCode::RegisterConst(const PyObjPtr& obj) {
  if (!consts->Contains(obj)) {
    consts->Append(obj);
  }
}

Index PyCode::IndexOfName(const PyObjPtr& _name) {
  if (!names->Contains(_name)) {
    Function::DebugPrint(_name);
    throw std::runtime_error("PyCode::IndexOfName(): name not found");
  }
  return names->IndexOf(_name);
}

void PyCode::RegisterName(const PyObjPtr& _name) {
  if (!names->Contains(_name)) {
    names->Append(_name);
  }
}

Index PyCode::IndexOfVarName(const PyObjPtr& _name) {
  if (!varNames->Contains(_name)) {
    throw std::runtime_error("PyCode::IndexOfVarName(): name not found");
  }
  return varNames->IndexOf(_name);
}

void PyCode::RegisterVarName(const PyObjPtr& _name) {
  if (!varNames->Contains(_name)) {
    varNames->Append(_name);
  }
}

PyCodePtr CreatePyCode(const PyStrPtr& name) {
  auto byteCode = CreatePyString("")->as<PyBytes>();
  auto consts = CreatePyList();
  auto names = CreatePyList();
  auto varNames = CreatePyList();
  return std::make_shared<PyCode>(
    byteCode, consts, names, varNames, name, 0, false
  );
}

void PrintCode(const PyCodePtr& code) {
  auto codeObj = code->as<PyCode>();
  VerboseTerminal::get_instance().info(
    codeObj->str()->as<PyString>()->ToCppString()
  );
  VerboseTerminal::IncreaseIndent();

  VerboseTerminal::get_instance().info("name: ");
  VerboseTerminal::get_instance().info(
    codeObj->Name()->str()->as<PyString>()->ToCppString()
  );

  VerboseTerminal::get_instance().info("consts: ");
  VerboseTerminal::get_instance().info(
    codeObj->Consts()->str()->as<PyString>()->ToCppString()
  );

  VerboseTerminal::get_instance().info("names: ");
  VerboseTerminal::get_instance().info(
    codeObj->Names()->str()->as<PyString>()->ToCppString()
  );

  VerboseTerminal::get_instance().info("varNames: ");
  VerboseTerminal::get_instance().info(
    codeObj->VarNames()->str()->as<PyString>()->ToCppString()
  );

  VerboseTerminal::get_instance().info("instructions:");
  VerboseTerminal::IncreaseIndent();

  for (Index i = 0; i < codeObj->Instructions()->Length(); i++) {
    auto inst = codeObj->Instructions()->GetItem(i);
    std::string line = std::to_string(i) + ": " +
                       inst->str()->as<PyString>()->ToCppString() + "";
    VerboseTerminal::get_instance().info(line);
  }

  VerboseTerminal::DecreaseIndent();
  VerboseTerminal::get_instance().info("nLocals: ");
  VerboseTerminal::get_instance().info(std::to_string(codeObj->NLocals()) + "");

  VerboseTerminal::DecreaseIndent();
}

}  // namespace kaubo::Object