#pragma once

#include "Collections/Matrix.h"
#include "Object/Container/PyList.h"
#include "Object/Core/Klass.h"
#include "Object/Core/PyObject.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
namespace kaubo::Object {

class MatrixKlass : public KlassBase<MatrixKlass> {
 public:
  explicit MatrixKlass() = default;
  void Initialize() override;

  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override;

  PyObjPtr matmul(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr mul(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr add(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr sub(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr truediv(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr gt(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;

  PyObjPtr neg(const PyObjPtr& obj) override;

  PyObjPtr len(const PyObjPtr& obj) override;

  PyObjPtr setitem(
    const PyObjPtr& obj,
    const PyObjPtr& key,
    const PyObjPtr& value
  ) override;

  PyObjPtr getitem(const PyObjPtr& obj, const PyObjPtr& key) override;
  PyObjPtr pow(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
};

class PyMatrix;
using PyMatrixPtr = std::shared_ptr<PyMatrix>;

inline PyMatrixPtr CreatePyMatrix(Collections::Matrix matrix) {
  return std::make_shared<PyMatrix>(std::move(matrix));
}

class PyMatrix : public PyObject {
  friend class MatrixKlass;

 private:
  Collections::Matrix matrix;

 public:
  explicit PyMatrix(Collections::Matrix matrix)
    : PyObject(MatrixKlass::Self()), matrix(std::move(matrix)) {}

  void Shuffle() { matrix.Shuffle(); }
  PyMatrixPtr Transpose() const { return CreatePyMatrix(matrix.Transpose()); }
  PyMatrixPtr MatrixMultiply(const PyMatrixPtr& other) {
    return CreatePyMatrix(matrix.MatrixMultiply(other->matrix));
  }
  PyMatrixPtr Multiply(const PyMatrixPtr& other) const {
    return CreatePyMatrix(matrix.Multiply(other->matrix));
  }
  PyMatrixPtr Multiply(double scalar) const {
    return CreatePyMatrix(matrix.Multiply(scalar));
  }
  PyMatrixPtr Divide(double scalar) const {
    return CreatePyMatrix(matrix.Divide(scalar));
  }
  PyMatrixPtr Add(const PyMatrixPtr& other) const {
    return CreatePyMatrix(matrix.Add(other->matrix));
  }
  PyMatrixPtr Subtract(const PyMatrixPtr& other) const {
    return CreatePyMatrix(matrix.Add(other->matrix.Multiply(-1)));
  }
  PyStrPtr ToString() const {
    return PyString::Create(matrix.ToString())->as<PyString>();
  }
  PyListPtr Shape() const {
    return PyList::Create<Object::PyObjPtr>({CreatePyInteger(matrix.Shape()[0]),
                                             CreatePyInteger(matrix.Shape()[1])}
    )
      ->as<PyList>();
  }
  PyIntPtr Rows() const {
    return CreatePyInteger(matrix.Rows())->as<PyInteger>();
  }
  PyIntPtr Cols() const {
    return CreatePyInteger(matrix.Cols())->as<PyInteger>();
  }
  Index RowsIndex() const { return matrix.Rows(); }
  Index ColsIndex() const { return matrix.Cols(); }

  PyMatrixPtr Reshape(Index rows, Index cols) const {
    return CreatePyMatrix(matrix.Reshape(rows, cols));
  }
  void SetRows(Index start, Index stop, const PyMatrixPtr& other) {
    matrix.SetRows(start, stop, other->matrix);
  }
  void SetCols(Index start, Index stop, const PyMatrixPtr& other) {
    matrix.SetCols(start, stop, other->matrix);
  }
  void SetSlice(
    Index rowStart,
    Index colStart,
    Index rowStop,
    Index colStop,
    const PyMatrixPtr& other
  ) {
    matrix.SetSlice(rowStart, colStart, rowStop, colStop, other->matrix);
  }
  PyMatrixPtr GetRows(Index start, Index stop) const {
    return CreatePyMatrix(matrix.GetRows(start, stop));
  }
  PyMatrixPtr GetCols(Index start, Index stop) const {
    return CreatePyMatrix(matrix.GetCols(start, stop));
  }
  PyMatrixPtr
  GetSlice(Index rowStart, Index colStart, Index rowStop, Index colStop) const {
    return CreatePyMatrix(matrix.GetSlice(rowStart, colStart, rowStop, colStop)
    );
  }
  double At(Index row, Index col) const { return matrix.At(row, col); }
  void Set(Index row, Index col, double value) { matrix.Set(row, col, value); }

  const Collections::List<double>& Ravel() const { return matrix.Data(); }
};

using PyMatrixPtr = std::shared_ptr<PyMatrix>;

}  // namespace kaubo::Object
