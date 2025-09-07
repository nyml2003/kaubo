#pragma once

#include "Collections/List.h"
#include "Collections/String/String.h"
namespace kaubo::Collections {

class Matrix {
 private:
  Index rows{};
  Index cols{};
  List<double> data;

  [[nodiscard]] uint8_t CheckBroadcast(const Matrix& other) const;

  [[nodiscard]] double
  BroadcastAt(Index row, Index col, uint8_t broadcast_type) const;

 public:
  [[nodiscard]] String ToString() const;
  explicit Matrix(Index rows, Index cols);
  explicit Matrix(Index rows, Index cols, List<double> data);
  explicit Matrix(List<List<double>> data);

  void Set(Index row, Index col, double value);
  void Shuffle();

  [[nodiscard]] Matrix Transpose() const;

  [[nodiscard]] Matrix Add(const Matrix& other) const;
  [[nodiscard]] Matrix Multiply(const Matrix& other) const;
  [[nodiscard]] Matrix Multiply(double scalar) const;
  [[nodiscard]] Matrix Divide(double scalar) const;
  [[nodiscard]] Index Rows() const;
  [[nodiscard]] Index Cols() const;
  [[nodiscard]] const List<double>& Data() const;
  [[nodiscard]] List<Index> Shape() const;
  [[nodiscard]] double At(Index row, Index col) const;
  [[nodiscard]] Matrix Reshape(Index newRows, Index newCols) const;

  [[nodiscard]] Matrix MatrixMultiply(const Matrix& other) const;

  static Matrix Eye(Index n);

  // 获取start行到stop行
  [[nodiscard]] Matrix GetRows(Index start, Index stop) const;

  // 获取start列到stop列
  [[nodiscard]] Matrix GetCols(Index start, Index stop) const;

  // 获取(rowStart, colStart)到(rowStop, colStop)的矩阵
  [[nodiscard]] Matrix
  GetSlice(Index rowStart, Index colStart, Index rowStop, Index colStop) const;

  // 设置start行到stop行
  void SetRows(Index start, Index stop, const Matrix& other);

  // 设置start列到stop列
  void SetCols(Index start, Index stop, const Matrix& other);

  // 设置(rowStart, colStart)到(rowStop, colStop)的矩阵
  void SetSlice(
    Index rowStart,
    Index colStart,
    Index rowStop,
    Index colStop,
    const Matrix& other
  );

  [[nodiscard]] Matrix Copy() const;
};

}  // namespace kaubo::Collections
