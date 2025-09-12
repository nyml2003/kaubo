#include "PyMatrix.h"
#include <cassert>

#include "MatrixFunction.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Function/PyIife.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Number/PyFloat.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {
PyObjPtr MatrixKlass::repr(const PyObjPtr& obj) {
  return Klass::repr(obj);
}
PyObjPtr MatrixKlass::str(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("MatrixKlass::repr(): obj is not a matrix");
  }
  auto matrix = obj->as<PyMatrix>();
  auto shape = matrix->Shape();
  auto rows = shape->GetItem(0)->as<PyInteger>()->ToU64();
  auto cols = shape->GetItem(1)->as<PyInteger>()->ToU64();
  auto repr = CreatePyString("[")->as<PyString>();
  for (Index i = 0; i < rows; i++) {
    repr = repr->Add(CreatePyString("[")->as<PyString>());
    for (Index j = 0; j < cols; j++) {
      repr = repr->Add(CreatePyString(" ")->as<PyString>());
      repr = repr->Add(CreatePyFloat(matrix->At(i, j))->repr()->as<PyString>());
      if (j != cols - 1) {
        repr = repr->Add(CreatePyString(", ")->as<PyString>());
      }
    }
    repr = repr->Add(CreatePyString("]")->as<PyString>());
    if (i != rows - 1) {
      repr = repr->Add(CreatePyString("\n ")->as<PyString>());
    }
  }
  repr = repr->Add(CreatePyString("]")->as<PyString>());
  return repr;
}

PyObjPtr MatrixKlass::matmul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error(
      "MatrixKlass::matmul(): lhs or rhs is not a matrix"
    );
  }
  return lhs->as<PyMatrix>()->MatrixMultiply(rhs->as<PyMatrix>());
}

PyObjPtr MatrixKlass::gt(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self())) {
    throw std::runtime_error("MatrixKlass::gt(): lhs is not a matrix");
  }
  if (!rhs->is(FloatKlass::Self())) {
    throw std::runtime_error("MatrixKlass::gt(): rhs is not a float");
  }
  auto matrix = lhs->as<PyMatrix>();
  auto compareObj = rhs->as<PyFloat>()->Value();
  auto shape = lhs->as<PyMatrix>()->Shape();
  auto rows = shape->GetItem(0)->as<PyInteger>()->ToU64();
  auto cols = shape->GetItem(1)->as<PyInteger>()->ToU64();
  // 初始化一个rows * cols的二维Python列表，用于存储比较结果
  Collections::List<PyObjPtr> data;
  for (Index i = 0; i < rows; i++) {
    Collections::List<PyObjPtr> row;
    for (Index j = 0; j < cols; j++) {
      row.Push(
        matrix->At(i, j) > compareObj ? PyBoolean::create(true)
                                      : PyBoolean::create(false)
      );
    }
    data.Push(CreatePyList(row));
  }
  return CreatePyList(data);
}

PyObjPtr MatrixKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(FloatKlass::Self())) {
    throw std::runtime_error("MatrixKlass::eq(): lhs or rhs is not a matrix");
  }
  auto matrix = lhs->as<PyMatrix>();
  auto compareObj = rhs->as<PyFloat>()->Value();
  auto shape = lhs->as<PyMatrix>()->Shape();
  auto rows = shape->GetItem(0)->as<PyInteger>()->ToU64();
  auto cols = shape->GetItem(1)->as<PyInteger>()->ToU64();
  Collections::List<PyObjPtr> data;
  for (Index i = 0; i < rows; i++) {
    Collections::List<PyObjPtr> row;
    for (Index j = 0; j < cols; j++) {
      row.Push(
        matrix->At(i, j) == compareObj ? PyBoolean::create(true)
                                       : PyBoolean::create(false)
      );
    }
    data.Push(CreatePyList(row));
  }
  return CreatePyList(data);
}

PyObjPtr MatrixKlass::mul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self())) {
    throw std::runtime_error("MatrixKlass::mul(): lhs or rhs is not a matrix");
  }
  auto matrix = lhs->as<PyMatrix>();
  if (rhs->is(Self())) {
    return matrix->Multiply(rhs->as<PyMatrix>());
  }
  if (rhs->is(FloatKlass::Self())) {
    return matrix->Multiply(rhs->as<PyFloat>()->Value());
  }
  throw std::runtime_error("MatrixKlass::mul(): rhs is not a matrix or float");
}

PyObjPtr MatrixKlass::add(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("MatrixKlass::add(): lhs or rhs is not a matrix");
  }
  return lhs->as<PyMatrix>()->Add(rhs->as<PyMatrix>());
}

PyObjPtr MatrixKlass::sub(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self()) || !rhs->is(Self())) {
    throw std::runtime_error("MatrixKlass::sub(): lhs or rhs is not a matrix");
  }
  return lhs->as<PyMatrix>()->Subtract(rhs->as<PyMatrix>());
}

PyObjPtr MatrixKlass::truediv(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self())) {
    throw std::runtime_error("MatrixKlass::floordiv(): lhs is not a matrix");
  }
  auto matrix = lhs->as<PyMatrix>();
  if (rhs->is(FloatKlass::Self())) {
    return matrix->Divide(rhs->as<PyFloat>()->Value());
  }
  if (rhs->is(IntegerKlass::Self())) {
    return matrix->Divide(static_cast<double>(rhs->as<PyInteger>()->ToI64()));
  }
  throw std::runtime_error(
    "MatrixKlass::floordiv(): rhs is not a matrix or float"
  );
}

PyObjPtr MatrixKlass::len(const PyObjPtr& obj) {
  if (!obj->is(Self())) {
    throw std::runtime_error("MatrixKlass::len(): obj is not a matrix");
  }
  return obj->as<PyMatrix>()->Rows();
}

PyObjPtr MatrixKlass::getitem(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(Self())) {
    throw std::runtime_error("MatrixKlass::getitem(): obj is not a matrix");
  }
  auto matrix = obj->as<PyMatrix>();
  // a[row,col] a[row] a[row][col] a[rowStart:rowEnd,colStart:colEnd]
  if (key->is(ListKlass::Self())) {
    // a[row,col] 或者 a[rowStart:rowEnd,colStart:colEnd]
    auto keyList = key->as<PyList>();
    if (keyList->GetItem(0)->is(IntegerKlass::Self()) &&
        keyList->GetItem(1)->is(IntegerKlass::Self())) {
      auto shape = matrix->Shape();
      auto matrixRow = shape->GetItem(0)->as<PyInteger>()->ToU64();
      auto matrixCol = shape->GetItem(1)->as<PyInteger>()->ToU64();
      Index rowValue = 0;
      Index colValue = 0;
      auto row = keyList->GetItem(0)->as<PyInteger>();
      if (row->GetSign() == Collections::Integer::IntSign::Positive) {
        rowValue = row->ToU64();
      }
      if (row->GetSign() == Collections::Integer::IntSign::Negative) {
        int64_t temp = static_cast<int64_t>(matrixRow) + row->ToI64();
        assert(temp >= 0 && "Negative index not allowed");
        rowValue = static_cast<Index>(temp);
      }
      auto col = keyList->GetItem(1)->as<PyInteger>();
      if (col->GetSign() == Collections::Integer::IntSign::Positive) {
        colValue = col->ToU64();
      }
      if (col->GetSign() == Collections::Integer::IntSign::Negative) {
        int64_t temp = static_cast<int64_t>(matrixCol) + col->ToI64();
        assert(temp >= 0 && "Negative index not allowed");
        colValue = static_cast<Index>(temp);
      }
      return CreatePyFloat(matrix->At(rowValue, colValue));
    }
    if (keyList->GetItem(0)->is(SliceKlass::Self()) &&
        keyList->GetItem(1)->is(SliceKlass::Self())) {
      // a[rowStart:rowEnd,colStart:colEnd]
      if (keyList->Length() != 2) {
        throw std::runtime_error("MatrixKlass::getitem(): key length is not 2");
      }
      auto rowSlice = keyList->GetItem(0)->as<PySlice>();
      auto shape = matrix->Shape();
      rowSlice->BindLength(matrix->Shape()->GetItem(0)->as<PyInteger>()->ToU64()
      );
      auto colSlice = keyList->GetItem(1)->as<PySlice>();
      colSlice->BindLength(matrix->Shape()->GetItem(1)->as<PyInteger>()->ToU64()
      );
      auto rowStart = rowSlice->GetStart()->as<PyInteger>()->ToU64();
      auto rowStop = rowSlice->GetStop()->as<PyInteger>()->ToU64();
      auto colStart = colSlice->GetStart()->as<PyInteger>()->ToU64();
      auto colStop = colSlice->GetStop()->as<PyInteger>()->ToU64();
      return matrix->GetSlice(rowStart, colStart, rowStop, colStop);
    }
    if (keyList->GetItem(0)->is(SliceKlass::Self()) &&
        keyList->GetItem(1)->is(IntegerKlass::Self())) {
      // a[rowStart:rowEnd,col]
      auto rowSlice = keyList->GetItem(0)->as<PySlice>();
      auto shape = matrix->Shape();
      rowSlice->BindLength(matrix->Shape()->GetItem(0)->as<PyInteger>()->ToU64()
      );
      auto matrixCol = shape->GetItem(1)->as<PyInteger>()->ToU64();
      auto col = keyList->GetItem(1)->as<PyInteger>();
      Index colValue = 0;
      if (col->GetSign() == Collections::Integer::IntSign::Positive) {
        colValue = col->ToU64();
      }
      if (col->GetSign() == Collections::Integer::IntSign::Negative) {
        int64_t temp = static_cast<int64_t>(matrixCol) + col->ToI64();
        assert(temp >= 0 && "Negative index not allowed");
        colValue = static_cast<Index>(temp);
      }
      auto rowStart = rowSlice->GetStart()->as<PyInteger>()->ToU64();
      auto rowStop = rowSlice->GetStop()->as<PyInteger>()->ToU64();
      return matrix->GetSlice(rowStart, colValue, rowStop, colValue + 1);
    }
    if (keyList->GetItem(0)->is(IntegerKlass::Self()) &&
        keyList->GetItem(1)->is(SliceKlass::Self())) {
      // a[row,colStart:colEnd]
      auto colSlice = keyList->GetItem(1)->as<PySlice>();
      auto shape = matrix->Shape();
      colSlice->BindLength(matrix->Shape()->GetItem(1)->as<PyInteger>()->ToU64()
      );
      auto row = keyList->GetItem(0)->as<PyInteger>();
      Index rowValue = 0;
      if (row->GetSign() == Collections::Integer::IntSign::Positive) {
        rowValue = row->ToU64();
      }
      if (row->GetSign() == Collections::Integer::IntSign::Negative) {
        int64_t temp =
          static_cast<int64_t>(matrix->Rows()->as<PyInteger>()->ToU64()) +
          row->ToI64();
        assert(temp >= 0 && "Negative index not allowed");
        rowValue = static_cast<Index>(temp);
      }
      auto colStart = colSlice->GetStart()->as<PyInteger>()->ToU64();
      auto colStop = colSlice->GetStop()->as<PyInteger>()->ToU64();
      return matrix->GetSlice(rowValue, colStart, rowValue + 1, colStop);
    }
  }
  if (key->is(SliceKlass::Self())) {
    // a[rowStart:rowEnd]
    auto rowSlice = key->as<PySlice>();
    rowSlice->BindLength(matrix->Shape()->GetItem(0)->as<PyInteger>()->ToU64());
    auto rowStart = rowSlice->GetStart()->as<PyInteger>()->ToU64();
    auto rowStop = rowSlice->GetStop()->as<PyInteger>()->ToU64();
    return matrix->GetRows(rowStart, rowStop);
  }
  throw std::runtime_error("MatrixKlass::getitem(): key type is not supported");
}

PyObjPtr MatrixKlass::setitem(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  if (!obj->is(Self())) {
    throw std::runtime_error("MatrixKlass::setitem(): obj is not a matrix");
  }
  if (!key->is(ListKlass::Self())) {
    throw std::runtime_error("MatrixKlass::setitem(): key is not a list");
  }
  auto matrix = obj->as<PyMatrix>();
  auto keyList = key->as<PyList>();
  // a[rowStart:rowStop,colStart:colStop] = matrix
  if (value->is(Self())) {
    auto other = value->as<PyMatrix>();
    auto rowSlice = keyList->GetItem(0)->as<PySlice>();
    rowSlice->BindLength(matrix->Shape()->GetItem(0)->as<PyInteger>()->ToU64());
    auto colSlice = keyList->GetItem(1)->as<PySlice>();
    colSlice->BindLength(matrix->Shape()->GetItem(1)->as<PyInteger>()->ToU64());
    auto rowStart = rowSlice->GetStart()->as<PyInteger>()->ToU64();
    auto rowStop = rowSlice->GetStop()->as<PyInteger>()->ToU64();
    auto colStart = colSlice->GetStart()->as<PyInteger>()->ToU64();
    auto colStop = colSlice->GetStop()->as<PyInteger>()->ToU64();
    matrix->SetSlice(rowStart, colStart, rowStop, colStop, other);
    return CreatePyNone();
  }
  // a[row,col] = float
  if (value->is(FloatKlass::Self())) {
    auto row = keyList->GetItem(0)->as<PyInteger>()->ToU64();
    auto col = keyList->GetItem(1)->as<PyInteger>()->ToU64();
    matrix->Set(row, col, value->as<PyFloat>()->Value());
    return CreatePyNone();
  }
  throw std::runtime_error(
    "MatrixKlass::setitem(): value type is not supported"
  );
  return CreatePyNone();
}

PyObjPtr MatrixKlass::pow(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(Self())) {
    throw std::runtime_error("MatrixKlass::pow(): lhs is not a matrix");
  }
  if (!rhs->is(IntegerKlass::Self())) {
    throw std::runtime_error("MatrixKlass::pow(): rhs is not an integer");
  }
  // 快速幂
  auto matrix = lhs->as<PyMatrix>();
  auto power = rhs->as<PyInteger>()->ToU64();
  if (power < 0) {
    throw std::runtime_error("MatrixKlass::pow(): n is less than 0");
  }
  auto result = Eye(CreatePyList({lhs->as<PyMatrix>()->Shape()->GetItem(0)}))
                  ->as<PyMatrix>();
  // 快速幂算法
  while (power != 0) {
    if ((power & 1ULL) != 0) {
      result = result->MatrixMultiply(matrix);
    }
    matrix = matrix->MatrixMultiply(matrix);
    power >>= 1ULL;
  }
  return result;
}

void MatrixKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  auto* instance = Self();
  InitKlass(CreatePyString("matrix")->as<PyString>(), instance);
  instance->AddAttribute(
    CreatePyString("T")->as<PyString>(), CreatePyIife(Transpose)
  );
  instance->AddAttribute(
    CreatePyString("shape")->as<PyString>(), CreatePyIife(Shape)
  );
  instance->AddAttribute(
    CreatePyString("reshape")->as<PyString>(), CreatePyNativeFunction(Reshape)
  );
  instance->AddAttribute(
    CreatePyString("ravel")->as<PyString>(), CreatePyNativeFunction(Ravel)
  );
  this->SetInitialized();
}

PyObjPtr MatrixKlass::neg(const kaubo::Object::PyObjPtr& obj) {
  auto matrix = obj->as<PyMatrix>();
  return matrix->Multiply(-1);
}

}  // namespace kaubo::Object