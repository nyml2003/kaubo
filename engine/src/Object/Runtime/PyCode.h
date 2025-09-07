#pragma once

#include <cstdint>
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/Klass.h"
#include "Object/Core/PyObject.h"
#include "Object/Object.h"
#include "Object/Runtime/PyInst.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class PyCode;

using PyCodePtr = std::shared_ptr<PyCode>;
enum class Scope : uint8_t { ERR = 0, LOCAL, GLOBAL, Closure };

class PyCode : public PyObject {
  friend class CodeKlass;

 public:
  explicit PyCode(
    PyBytesPtr byteCodes,
    PyListPtr consts,
    PyListPtr names,
    PyListPtr varNames,
    PyStrPtr name,
    Index nLocals,
    bool isGenerator
  );

  [[nodiscard]] PyListPtr Instructions();

  void SetInstructions(PyListPtr&& insts);

  void SetByteCode(const PyBytesPtr& byteCodes);

  void SetNLocals(Index nLocals);

  void SetScope(Scope scope);

  PyBytesPtr ByteCode() const;

  [[nodiscard]] PyListPtr Consts() const;

  [[nodiscard]] PyListPtr Names() const;

  [[nodiscard]] PyStrPtr Name() const;

  [[nodiscard]] PyListPtr VarNames() const;

  [[nodiscard]] Index NLocals() const;

  [[nodiscard]] Scope GetScope() const;

  void EnableGenerator() { isGenerator = true; }

  bool IsGenerator() const { return isGenerator; }

  Index IndexOfConst(const PyObjPtr& obj);

  void RegisterConst(const PyObjPtr& obj);

  Index IndexOfName(const PyObjPtr& name);

  void RegisterName(const PyObjPtr& name);

  Index IndexOfVarName(const PyObjPtr& name);

  void RegisterVarName(const PyObjPtr& name);

  /**
   * @param obj 可以是String, Integer, Float, Boolean, None, List，Code
   */
  void LoadConst(const PyObjPtr& obj) {
    auto index = IndexOfConst(obj);
    instructions->Append(MakeInst<ByteCode::LOAD_CONST>(index));
  }

  void LoadName(const PyObjPtr& obj) {
    auto index = IndexOfName(obj);
    instructions->Append(MakeInst<ByteCode::LOAD_NAME>(index));
  }

  void StoreName(const PyObjPtr& obj) {
    auto index = IndexOfName(obj);
    instructions->Append(MakeInst<ByteCode::STORE_NAME>(index));
  }

  void LoadAttr(const PyObjPtr& obj) {
    auto index = IndexOfName(obj);
    instructions->Append(MakeInst<ByteCode::LOAD_ATTR>(index));
  }

  void LoadGlobal(const PyObjPtr& obj) {
    auto index = IndexOfName(obj);
    instructions->Append(MakeInst<ByteCode::LOAD_GLOBAL>(index));
  }

  void LoadFast(const PyObjPtr& obj) {
    auto index = IndexOfVarName(obj);
    instructions->Append(MakeInst<ByteCode::LOAD_FAST>(index));
  }

  void StoreFast(const PyObjPtr& obj) {
    auto index = IndexOfVarName(obj);
    instructions->Append(MakeInst<ByteCode::STORE_FAST>(index));
  }

  void BuildList(Index size) {
    instructions->Append(MakeInst<ByteCode::BUILD_LIST>(size));
  }

  void BuildSlice() { instructions->Append(MakeInst<ByteCode::BUILD_SLICE>()); }

  void BuildMap(Index size) {
    instructions->Append(MakeInst<ByteCode::BUILD_MAP>(size));
  }

  void CallFunction(Index nArgs) {
    instructions->Append(MakeInst<ByteCode::CALL_FUNCTION>(nArgs));
  }

  void MakeFunction() {
    instructions->Append(MakeInst<ByteCode::MAKE_FUNCTION>());
  }

  void ReturnValue() {
    instructions->Append(MakeInst<ByteCode::RETURN_VALUE>());
  }

  Index PopJumpIfFalse() {
    instructions->Append(MakeInst<ByteCode::POP_JUMP_IF_FALSE>(0));
    return instructions->Length();
  }

  Index JumpForward() {
    instructions->Append(MakeInst<ByteCode::JUMP_FORWARD>(0));
    return instructions->Length();
  }

  void JumpAbsolute(Index index) {
    instructions->Append(MakeInst<ByteCode::JUMP_ABSOLUTE>(index));
  }

  void PopTop() { instructions->Append(MakeInst<ByteCode::POP_TOP>()); }

  void StoreSubscr() {
    instructions->Append(MakeInst<ByteCode::STORE_SUBSCR>());
  }

  void GetIter() { instructions->Append(MakeInst<ByteCode::GET_ITER>()); }

  Index ForIter(Index index) {
    instructions->Append(MakeInst<ByteCode::FOR_ITER>(index));
    return instructions->Length();
  }

  void LoadBuildClass() {
    instructions->Append(MakeInst<ByteCode::LOAD_BUILD_CLASS>());
  }

  void StoreAttr(const PyObjPtr& obj) {
    auto index = IndexOfName(obj);
    instructions->Append(MakeInst<ByteCode::STORE_ATTR>(index));
  }

  void Nop() { instructions->Append(MakeInst<ByteCode::NOP>()); }

  void UnaryPositive() {
    instructions->Append(MakeInst<ByteCode::UNARY_POSITIVE>());
  }

  void UnaryNegative() {
    instructions->Append(MakeInst<ByteCode::UNARY_NEGATIVE>());
  }

  void UnaryNot() { instructions->Append(MakeInst<ByteCode::UNARY_NOT>()); }

  void UnaryInvert() {
    instructions->Append(MakeInst<ByteCode::UNARY_INVERT>());
  }

  void BinaryPower() {
    instructions->Append(MakeInst<ByteCode::BINARY_POWER>());
  }

  void BinaryModulo() {
    instructions->Append(MakeInst<ByteCode::BINARY_MODULO>());
  }

  void BinaryFloorDivide() {
    instructions->Append(MakeInst<ByteCode::BINARY_FLOOR_DIVIDE>());
  }

  void BinaryTrueDivide() {
    instructions->Append(MakeInst<ByteCode::BINARY_TRUE_DIVIDE>());
  }

  void BinaryLShift() {
    instructions->Append(MakeInst<ByteCode::BINARY_LSHIFT>());
  }

  void BinaryRShift() {
    instructions->Append(MakeInst<ByteCode::BINARY_RSHIFT>());
  }

  void BinaryAnd() { instructions->Append(MakeInst<ByteCode::BINARY_AND>()); }

  void BinaryXor() { instructions->Append(MakeInst<ByteCode::BINARY_XOR>()); }

  void BinaryOr() { instructions->Append(MakeInst<ByteCode::BINARY_OR>()); }

  void YieldValue() { instructions->Append(MakeInst<ByteCode::YIELD_VALUE>()); }

 private:
  PyBytesPtr byteCode;

  PyListPtr instructions;
  PyListPtr consts;
  PyListPtr names;
  PyListPtr varNames;
  PyStrPtr name;
  Index nLocals;
  bool isGenerator = false;
  enum Scope scope = Scope::ERR;
};

using PyCodePtr = std::shared_ptr<PyCode>;

class CodeKlass : public KlassBase<CodeKlass> {
 public:
  explicit CodeKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("code")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr repr(const PyObjPtr& self) override;

  PyObjPtr _serialize_(const PyObjPtr& self) override;

  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
};

PyCodePtr CreatePyCode(const PyStrPtr& name);
// 打印PyCode的详细信息
void PrintCode(const PyCodePtr& code);
inline PyObjPtr CreatePyCode(
  const PyBytesPtr& byteCode,
  const PyListPtr& consts,
  const PyListPtr& names,
  const PyListPtr& varNames,
  const PyStrPtr& name,
  Index nLocals,
  bool isGenerator
) {
  return std::make_shared<PyCode>(
    byteCode, consts, names, varNames, name, nLocals, isGenerator
  );
}

}  // namespace kaubo::Object
