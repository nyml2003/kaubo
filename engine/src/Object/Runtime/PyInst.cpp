#include "Object/Runtime/PyInst.h"
#include "Collections/String/BytesHelper.h"
#include "Collections/String/StringHelper.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyInst::PyInst(ByteCode code, OperandKind operand)
  : PyObject(InstKlass::Self()), code(code), operand(operand) {}

[[nodiscard]] ByteCode PyInst::Code() const {
  return code;
}

[[nodiscard]] OperandKind PyInst::Operand() const {
  return operand;
}

PyObjPtr InstKlass::_serialize_(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error(
      "PyInst::_serialize_(): obj is not an inst object"
    );
  }
  auto inst = obj->as<PyInst>();
  Collections::StringBuilder stringBuilder(
    Collections::Serialize(inst->Code())
  );
  std::visit(
    overload{
      [](None) {},
      [&stringBuilder](Index index) {
        stringBuilder.Append(Collections::Serialize(index));
      },
      [&stringBuilder](CompareOp compOp) {
        stringBuilder.Append(Collections::Serialize(compOp));
      },
      [&stringBuilder](int64_t index) {
        stringBuilder.Append(Collections::Serialize(index));
      }
    },
    inst->Operand()
  );
  return PyBytes::Create(stringBuilder.ToString());
}

PyObjPtr InstKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("PyInst::repr(): obj is not an inst object");
  }
  auto inst = obj->as<PyInst>();
  Collections::StringBuilder stringBuilder(Collections::ToString(inst->Code()));
  stringBuilder.Append(Collections::CreateStringWithCString(" "));
  std::visit(
    overload{
      [](None) {},
      [&stringBuilder](Index index) {
        stringBuilder.Append(Collections::ToString(index));
      },
      [&stringBuilder](CompareOp compOp) {
        stringBuilder.Append(Collections::ToString(compOp));
      },
      [&stringBuilder](int64_t index) {
        stringBuilder.Append(Collections::ToString(index));
      }
    },
    inst->Operand()
  );
  return PyString::Create(stringBuilder.ToString());
}

}  // namespace kaubo::Object