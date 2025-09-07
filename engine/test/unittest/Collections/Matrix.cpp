// NOLINTBEGIN(*)
#include "Collections/Matrix.h"
#include "../test_default.h"
#include "Collections/String/StringHelper.h"

using namespace kaubo::Collections;
using kaubo::Index;

TEST(Matrix, Constructor) {
  Matrix mat1(2, 3);
  ASSERT_EQ(mat1.Rows(), 2);
  ASSERT_EQ(mat1.Cols(), 3);
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(mat1.At(i, j), 0);
    }
  }

  List<double> data{1, 2, 3, 4, 5, 6};
  Matrix mat2(2, 3, data);
  ASSERT_EQ(mat2.Rows(), 2);
  ASSERT_EQ(mat2.Cols(), 3);
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(mat2.At(i, j), data[i * 3 + j]);
    }
  }

  List<List<double>> nested_data = {{1, 2, 3}, {4, 5, 6}};
  Matrix mat3(nested_data);
  ASSERT_EQ(mat3.Rows(), 2);
  ASSERT_EQ(mat3.Cols(), 3);
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(mat3.At(i, j), nested_data[i][j]);
    }
  }
}

TEST(Matrix, ToString) {
  List<List<double>> nested_data = {{1, 2, 3}, {4, 5, 6}};
  Matrix mat(nested_data);
  ASSERT_EQ(
    mat.ToString().ToCppString(),
    "[[1.000000,2.000000,3.000000][4.000000,5.000000,6.000000]]"
  );
}

TEST(Matrix, Transpose) {
  List<List<double>> nested_data = {{1, 2, 3}, {4, 5, 6}};
  Matrix mat(nested_data);
  Matrix transposed = mat.Transpose();
  List<List<double>> expected = {{1, 4}, {2, 5}, {3, 6}};
  for (Index i = 0; i < 3; ++i) {
    for (Index j = 0; j < 2; ++j) {
      ASSERT_EQ(transposed.At(i, j), expected[i][j]);
    }
  }
}

TEST(Matrix, Add) {
  Matrix mat1(2, 3, {1, 2, 3, 4, 5, 6});
  Matrix mat2(2, 3, {6, 5, 4, 3, 2, 1});
  Matrix result = mat1.Add(mat2);
  List<double> expected = {7, 7, 7, 7, 7, 7};
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(result.At(i, j), expected[i * 3 + j]);
    }
  }

  Matrix mat3(1, 3, {1, 2, 3});
  Matrix mat4(2, 1, {4, 5});
  result = mat3.Add(mat4);
  List<double> expected2 = {5, 6, 7, 6, 7, 8};
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(result.At(i, j), expected2[i * 3 + j]);
    }
  }
}

TEST(Matrix, Multiply) {
  Matrix mat1(2, 3, {1, 2, 3, 4, 5, 6});
  Matrix mat2(2, 3, {6, 5, 4, 3, 2, 1});
  Matrix result = mat1.Multiply(mat2);
  List<double> expected = {6, 10, 12, 12, 10, 6};
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(result.At(i, j), expected[i * 3 + j]);
    }
  }

  Matrix mat3(1, 3, {1, 2, 3});
  Matrix mat4(2, 1, {4, 5});
  result = mat3.Multiply(mat4);
  List<double> expected2{4, 8, 12, 5, 10, 15};
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(result.At(i, j), expected2[i * 3 + j]);
    }
  }
}

TEST(Matrix, MultiplyScalar) {
  Matrix mat(2, 3, {1, 2, 3, 4, 5, 6});
  Matrix result = mat.Multiply(2);
  List<double> expected = {2, 4, 6, 8, 10, 12};
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(result.At(i, j), expected[i * 3 + j]);
    }
  }
}

TEST(Matrix, Reshape) {
  Matrix mat(2, 3, {1, 2, 3, 4, 5, 6});
  Matrix reshaped = mat.Reshape(3, 2);
  List<double> expected = {1, 2, 3, 4, 5, 6};
  for (Index i = 0; i < 3; ++i) {
    for (Index j = 0; j < 2; ++j) {
      ASSERT_EQ(reshaped.At(i, j), expected[i * 2 + j]);
    }
  }
}

TEST(Matrix, MatrixMultiply) {
  Matrix mat1(2, 3, {1, 2, 3, 4, 5, 6});
  Matrix mat2(3, 2, {7, 8, 9, 10, 11, 12});
  Matrix result = mat1.MatrixMultiply(mat2);
  List<double> expected = {58, 64, 139, 154};
  for (Index i = 0; i < 2; ++i) {
    for (Index j = 0; j < 2; ++j) {
      ASSERT_EQ(result.At(i, j), expected[i * 2 + j]);
    }
  }
}

TEST(Matrix, Eye) {
  Matrix identity = Matrix::Eye(3);
  List<double> expected = {1, 0, 0, 0, 1, 0, 0, 0, 1};
  for (Index i = 0; i < 3; ++i) {
    for (Index j = 0; j < 3; ++j) {
      ASSERT_EQ(identity.At(i, j), expected[i * 3 + j]);
    }
  }
}

// NOLINTEND(*)