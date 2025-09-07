
#include "Collections/Matrix.h"
#include "Collections/String/StringHelper.h"

namespace kaubo::Collections {

Matrix::Matrix(Index rows, Index cols)
  : rows(rows), cols(cols), data(rows * cols) {
  data.Fill(0);
}

Matrix::Matrix(Index rows, Index cols, List<double> data)
  : rows(rows), cols(cols), data(std::move(data)) {}

Matrix::Matrix(List<List<double>> data) {
  if (data.Empty()) {
    throw std::invalid_argument("Matrix::Matrix: data is empty");
  }
  rows = data.Size();
  cols = data[0].Size();
  for (Index i = 1; i < rows; i++) {
    if (data[i].Size() != cols) {
      throw std::invalid_argument("Matrix::Matrix: data is not rectangular");
    }
  }
  this->data = List<double>(rows * cols);
  this->data.Fill(0);
  for (Index i = 0; i < rows; i++) {
    for (Index j = 0; j < cols; j++) {
      this->data.Set((i * cols) + j, data[i][j]);
    }
  }
}

List<Index> Matrix::Shape() const {
  return List<Index>({rows, cols});
}

void Matrix::Set(Index row, Index col, double value) {
  if (row >= rows || col >= cols) {
    throw std::invalid_argument("Matrix::Set: Index out of range");
  }
  data.Set((row * cols) + col, value);
}

void Matrix::Shuffle() {
  Collections::List<Index> indices(rows, static_cast<Index>(0));
  for (Index i = 0; i < rows; i++) {
    indices.Set(i, i);
  }
  indices.Shuffle();
  Matrix newMatrix(rows, cols);
  for (Index i = 0; i < rows; i++) {
    newMatrix.SetRows(i, i + 1, GetRows(indices[i], indices[i] + 1));
  }
  this->data = newMatrix.data;
}

String Matrix::ToString() const {
  StringBuilder stringBuilder;
  stringBuilder.Append(CreateStringWithCString("["));
  for (Index i = 0; i < rows; i++) {
    stringBuilder.Append(CreateStringWithCString("["));
    for (Index j = 0; j < cols; j++) {
      stringBuilder.Append(Collections::ToString(At(i, j)));
      if (j != cols - 1) {
        stringBuilder.Append(CreateStringWithCString(","));
      }
    }
    stringBuilder.Append(CreateStringWithCString("]"));
  }
  stringBuilder.Append(CreateStringWithCString("]"));
  return stringBuilder.ToString();
}

Matrix Matrix::Transpose() const {
  Matrix result(cols, rows);
  for (Index i = 0; i < rows; i++) {
    for (Index j = 0; j < cols; j++) {
      result.data.Set((j * rows) + i, At(i, j));
    }
  }
  return result;
}
/**
 * @brief
 * @details
维度匹配：两个数组在进行操作时，它们的形状必须在所有维度上兼容。这意味着两个数组的形状在每个维度上要么相同，要么其中一个数组在该维度上的大小为1。

大小为1的维度：如果两个数组的形状在某个维度上不匹配，那么其中一个数组在该维度上的大小必须是1。这样，才能将该数组沿着该维度“广播”到匹配另一个数组的形状。

没有负数或零维度：不允许负数或零维度的数组进行广播。

没有维度上的重叠：如果两个数组在某个维度上的大小都大于1，那么它们在这个维度上的大小必须相同。

没有形状为(0,)的数组：不允许形状为(0,)的数组参与广播，因为它们没有有效的广播规则。

没有形状为(1,)和(0,)的数组组合：如果一个数组的形状为(1,)，另一个数组的形状为(0,)，不允许它们进行广播。

没有形状为(0,)和(0,)的数组组合：两个形状都为(0,)的数组不能进行广播，因为它们没有有效的广播规则。

没有形状为(0,)的数组与非(1,)形状的数组组合：一个形状为(0,)的数组不能与另一个在任何维度上大小不为1的数组进行广播。

没有形状为(1,)的数组与形状为(0,)的数组组合：一个形状为(1,)的数组不能与另一个形状为(0,)的数组进行广播。
 * @param other
 * @return 4位二进制数，从右到左依次表示：
 *         - 第1位：other是否可以在列上广播到this
 *         - 第2位：other是否可以在行上广播到this
 *         - 第3位：this是否可以在列上广播到other
 *         - 第4位：this是否可以在行上广播到other
 */
uint8_t Matrix::CheckBroadcast(const Matrix& other) const {
  if (rows == other.rows && cols == other.cols) {
    return 0b1111;
  }
  uint8_t t1 = static_cast<uint8_t>(cols != 1);
  uint8_t t2 = static_cast<uint8_t>(other.cols != 1);
  uint8_t t3 = static_cast<uint8_t>(rows != 1);
  uint8_t t4 = static_cast<uint8_t>(other.rows != 1);
  return static_cast<uint8_t>(
    (static_cast<uint8_t>(!t1 && t2) << 3) |
    (static_cast<uint8_t>(!t3 && t4) << 2) |
    (static_cast<uint8_t>(!t2 && t1) << 1) | static_cast<uint8_t>(!t4 && t3)
  );
}

double Matrix::BroadcastAt(Index row, Index col, uint8_t broadcast_type) const {
  return At(
    (broadcast_type & 0b01 ? 0 : row), (broadcast_type & 0b10 ? 0 : col)
  );
}

Matrix Matrix::Add(const Matrix& other) const {
  uint8_t broadcast_type = CheckBroadcast(other);
  if (broadcast_type == 0) {
    throw std::invalid_argument("Matrix::Add: Matrix dimensions must be equal");
  }
  if (broadcast_type == 0b1111) {
    Matrix result(rows, cols);
    for (Index i = 0; i < rows; i++) {
      for (Index j = 0; j < cols; j++) {
        result.data.Set((i * cols) + j, At(i, j) + other.At(i, j));
      }
    }
    return result;
  }
  uint8_t thisBroadcastType = (broadcast_type & 0b1100) >> 2;
  uint8_t otherBroadcastType = broadcast_type & 0b0011;
  Matrix result(std::max(rows, other.rows), std::max(cols, other.cols));
  for (Index i = 0; i < result.rows; i++) {
    for (Index j = 0; j < result.cols; j++) {
      result.data.Set(
        (i * result.cols) + j, BroadcastAt(i, j, thisBroadcastType) +
                                 other.BroadcastAt(i, j, otherBroadcastType)
      );
    }
  }
  return result;
}

Matrix Matrix::Multiply(const Matrix& other) const {
  uint8_t broadcast_type = CheckBroadcast(other);
  if (broadcast_type == 0) {
    throw std::invalid_argument("Matrix dimensions must be equal");
  }

  if (broadcast_type == 0b1111) {
    Matrix result(rows, cols);
    for (Index i = 0; i < rows; i++) {
      for (Index j = 0; j < cols; j++) {
        result.data.Set((i * cols) + j, At(i, j) * other.At(i, j));
      }
    }
    return result;
  }
  uint8_t thisBroadcastType = (broadcast_type & 0b1100) >> 2;
  uint8_t otherBroadcastType = broadcast_type & 0b0011;
  Matrix result(std::max(rows, other.rows), std::max(cols, other.cols));
  for (Index i = 0; i < result.rows; i++) {
    for (Index j = 0; j < result.cols; j++) {
      result.data.Set(
        (i * result.cols) + j, BroadcastAt(i, j, thisBroadcastType) *
                                 other.BroadcastAt(i, j, otherBroadcastType)
      );
    }
  }
  return result;
}

Matrix Matrix::Multiply(double scalar) const {
  Matrix result(rows, cols);
  for (Index i = 0; i < rows * cols; i++) {
    result.data.Set(i, data.Get(i) * scalar);
  }
  return result;
}

Matrix Matrix::Divide(double scalar) const {
  if (scalar == 0) {
    throw std::invalid_argument("Matrix::Divide: Division by zero");
  }
  Matrix result(rows, cols);
  for (Index i = 0; i < rows * cols; i++) {
    result.data.Set(i, data.Get(i) / scalar);
  }
  return result;
}

Index Matrix::Rows() const {
  return rows;
}

Index Matrix::Cols() const {
  return cols;
}

const List<double>& Matrix::Data() const {
  return data;
}

double Matrix::At(Index row, Index col) const {
  return data.Get((row * cols) + col);
}

Matrix Matrix::Reshape(Index newRows, Index newCols) const {
  if (newRows * newCols != rows * cols) {
    throw std::invalid_argument("Matrix dimensions must be equal");
  }
  return Matrix(newRows, newCols, data);
}

Matrix Matrix::MatrixMultiply(const Matrix& other) const {
  const Index M = rows;
  const Index N = other.cols;
  const Index K = cols;
  const Index tile_size = 64;  // 根据缓存大小调整（如 L1 缓存 32KB
                               // 时，tile_size=64 对于 double 类型）

  Matrix result(M, N);

  // 分块循环
  for (Index ii = 0; ii < M; ii += tile_size) {
    for (Index jj = 0; jj < N; jj += tile_size) {
      for (Index kk = 0; kk < K; kk += tile_size) {
        // 处理分块矩阵：A[ii..ii+tile_size][kk..kk+tile_size]
        // 与 B[kk..kk+tile_size][jj..jj+tile_size]
        for (Index i = ii; i < std::min(ii + tile_size, M); ++i) {
          for (Index k = kk; k < std::min(kk + tile_size, K); ++k) {
            // 内层循环 j 连续访问 B[k][j]
            for (Index j = jj; j < std::min(jj + tile_size, N); ++j) {
              result.Set(i, j, result.At(i, j) + (At(i, k) * other.At(k, j)));
            }
          }
        }
      }
    }
  }

  return result;
}

Matrix Matrix::Eye(Index n) {
  Matrix result(n, n);
  for (Index i = 0; i < n; i++) {
    result.data.Set((i * n) + i, 1);
  }
  return result;
}

Matrix Matrix::GetRows(Index start, Index stop) const {
  if (stop > rows) {
    throw std::out_of_range("Index out of range");
  }
  // 行优先，行完整，可以copy
  return Matrix(stop - start, cols, data.Slice(start * cols, stop * cols));
}

Matrix Matrix::GetCols(Index start, Index stop) const {
  if (stop > cols) {
    throw std::out_of_range("Index out of range");
  }
  List<double> newData;
  for (Index row = 0; row < rows; row++) {
    newData.Concat(data.Slice((row * cols) + start, (row * cols) + stop));
  }
  return Matrix(rows, stop - start, newData);
}

Matrix Matrix::GetSlice(
  Index rowStart,
  Index colStart,
  Index rowStop,
  Index colStop
) const {
  if (rowStop > rows || colStop > cols) {
    throw std::out_of_range("Index out of range");
  }
  List<double> newData;
  for (Index row = rowStart; row < rowStop; row++) {
    newData.Concat(data.Slice((row * cols) + colStart, (row * cols) + colStop));
  }
  return Matrix(rowStop - rowStart, colStop - colStart, newData);
}

void Matrix::SetRows(Index start, Index stop, const Matrix& other) {
  if (stop > rows || start > stop) {
    throw std::out_of_range("Index out of range");
  }
  if (other.rows != stop - start || other.cols != cols) {
    throw std::invalid_argument("Invalid other");
  }
  double* targetData = data.Data() + (start * cols);
  const double* sourceData = other.data.Data();
  std::copy(sourceData, sourceData + ((stop - start) * cols), targetData);
}
void Matrix::SetCols(Index start, Index stop, const Matrix& other) {
  if (stop > cols - 1 || start > stop) {
    throw std::out_of_range("Index out of range");
  }
  if (other.rows != rows || other.cols != stop - start) {
    throw std::invalid_argument("Invalid other matrix");
  }
  double* targetData = data.Data();
  const double* sourceData = other.data.Data();
  for (Index row = 0; row < rows; row++) {
    std::copy(
      sourceData + (row * other.cols),
      sourceData + (row * other.cols) + (stop - start),
      targetData + (row * cols) + start
    );
  }
}

void Matrix::SetSlice(
  Index rowStart,
  Index colStart,
  Index rowStop,
  Index colStop,
  const Matrix& other
) {
  // 检查边界是否合法
  if (rowStop > rows || colStop > cols) {
    throw std::out_of_range("Index out of range");
  }
  if (rowStart > rowStop || colStart > colStop) {
    throw std::invalid_argument("Invalid slice range");
  }

  // 检查 other 矩阵的大小是否与目标区域匹配
  Index targetRows = rowStop - rowStart;
  Index targetCols = colStop - colStart;
  if (other.rows != targetRows || other.cols != targetCols) {
    throw std::invalid_argument("Size mismatch between slice and other matrix");
  }

  // 获取目标矩阵和源矩阵的原始数据指针
  double* targetData = data.Data();
  const double* sourceData = other.data.Data();

  // 遍历目标区域的每一行
  for (Index row = 0; row < targetRows; ++row) {
    // 计算目标矩阵的起始位置
    double* targetRowStart = targetData + ((rowStart + row) * cols) + colStart;
    // 计算源矩阵的起始位置
    const double* sourceRowStart = sourceData + (row * other.cols);

    // 复制一行数据
    std::copy(sourceRowStart, sourceRowStart + targetCols, targetRowStart);
  }
}

Matrix Matrix::Copy() const {
  return Matrix(rows, cols, data);
}

}  // namespace kaubo::Collections
