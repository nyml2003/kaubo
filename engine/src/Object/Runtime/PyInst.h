#pragma once

#include "ByteCode/ByteCode.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class PyInst : public PyObject {
 private:
  ByteCode code;
  OperandKind operand = None();

 public:
  explicit PyInst(ByteCode code, OperandKind operand = None());

  [[nodiscard]] ByteCode Code() const;

  [[nodiscard]] OperandKind Operand() const;
};

using PyInstPtr = std::shared_ptr<PyInst>;

class InstKlass : public KlassBase<InstKlass> {
 public:
  explicit InstKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("inst")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr _serialize_(const PyObjPtr& obj) override;

  PyObjPtr repr(const PyObjPtr& obj) override;

  PyObjPtr str(const PyObjPtr& obj) override { return repr(obj); }
};

template <ByteCode Op>
struct InstTraits {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_ADD> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_SUBTRACT> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_MULTIPLY> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::STORE_NAME> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::LOAD_NAME> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::STORE_FAST> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::LOAD_FAST> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::COMPARE_OP> {
  using operand_type = CompareOp;
};

template <>
struct InstTraits<ByteCode::POP_JUMP_IF_FALSE> {
  using operand_type = int64_t;
};

template <>
struct InstTraits<ByteCode::POP_JUMP_IF_TRUE> {
  using operand_type = int64_t;
};

template <>
struct InstTraits<ByteCode::MAKE_FUNCTION> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::CALL_FUNCTION> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::RETURN_VALUE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::LOAD_GLOBAL> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::POP_TOP> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::LOAD_ATTR> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::BUILD_LIST> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::BUILD_SLICE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BUILD_MAP> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::BINARY_MATRIX_MULTIPLY> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::JUMP_ABSOLUTE> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::BINARY_SUBSCR> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::STORE_SUBSCR> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::GET_ITER> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::FOR_ITER> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::LOAD_BUILD_CLASS> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::STORE_ATTR> {
  using operand_type = Index;
};

template <>
struct InstTraits<ByteCode::NOP> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::UNARY_POSITIVE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::UNARY_NEGATIVE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::UNARY_NOT> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::UNARY_INVERT> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_POWER> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_MODULO> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_FLOOR_DIVIDE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_TRUE_DIVIDE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_XOR> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_AND> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_OR> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_LSHIFT> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::BINARY_RSHIFT> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::YIELD_VALUE> {
  using operand_type = void;
};

template <>
struct InstTraits<ByteCode::JUMP_FORWARD> {
  using operand_type = Index;
};

template <ByteCode Op, typename T = typename InstTraits<Op>::operand_type>
std::enable_if_t<std::is_same_v<T, void>, PyInstPtr> MakeInst() {
  return std::make_shared<PyInst>(Op);
}

template <ByteCode Op, typename T = typename InstTraits<Op>::operand_type>
std::enable_if_t<!std::is_same_v<T, void>, PyInstPtr> MakeInst(T&& value) {
  return std::make_shared<PyInst>(Op, std::forward<T>(value));
}

}  // namespace kaubo::Object
