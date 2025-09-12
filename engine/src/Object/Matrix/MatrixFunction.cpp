#include "Object/Matrix/MatrixFunction.h"
#include "Object/Container/PyList.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Matrix/PyMatrix.h"
#include "Object/Number/PyFloat.h"

namespace kaubo::Object {
PyObjPtr Array(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto argList = args->as<PyList>();
  auto arg = argList->GetItem(0);
  if (arg->is(ListKlass::Self())) {
    auto list = arg->as<PyList>();
    auto rows = list->Length();
    auto cols = list->GetItem(0)->as<PyList>()->Length();
    Collections::List<double> data;
    for (Index i = 0; i < rows; i++) {
      auto row = list->GetItem(i)->as<PyList>();
      if (row->Length() != cols) {
        throw std::runtime_error("Matrix(): row length is not equal to cols");
      }
      for (Index j = 0; j < cols; j++) {
        if (!row->GetItem(j)->is(FloatKlass::Self())) {
          throw std::runtime_error("Matrix(): element is not a float");
        }
        auto element = row->GetItem(j)->as<PyFloat>();
        data.Push(element->Value());
      }
    }
    return CreatePyMatrix(Collections::Matrix(rows, cols, data));
  }
  if (arg->is(FloatKlass::Self())) {
    auto value = arg->as<PyFloat>()->Value();
    Collections::List<double> data(1, value);
    return CreatePyMatrix(Collections::Matrix(1, 1, data));
  }
  if (arg->is(IntegerKlass::Self())) {
    auto value = arg->as<PyInteger>()->ToU64();
    Collections::List<double> data(1, static_cast<double>(value));
    return CreatePyMatrix(Collections::Matrix(1, 1, data));
  }
  if (arg->is(MatrixKlass::Self())) {
    return arg;
  }
  throw std::runtime_error(
    "Matrix(): arg is not a list or float" +
    arg->str()->as<PyString>()->ToCppString()
  );
}

PyObjPtr Eye(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Eye(): args is not a list");
  }
  auto argList = args->as<PyList>();

  if (argList->Length() != 1) {
    throw std::runtime_error("Eye(): args length is not 1");
  }
  auto dim = argList->GetItem(0)->as<PyInteger>()->ToU64();
  return CreatePyMatrix(Collections::Matrix::Eye(dim));
}

PyObjPtr Zeros(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Zeros(): args is not a list");
  }
  auto argList = args->as<PyList>();

  if (argList->Length() != 1) {
    throw std::runtime_error("Zeros(): args length is not 1");
  }
  auto shape = argList->GetItem(0)->as<PyList>();
  if (shape->Length() != 2) {
    throw std::runtime_error("Zeros(): shape length is not 2");
  }
  auto rows = shape->GetItem(0)->as<PyInteger>()->ToU64();
  auto cols = shape->GetItem(1)->as<PyInteger>()->ToU64();
  Collections::List<double> data(rows * cols, 0.0);
  return CreatePyMatrix(Collections::Matrix(rows, cols, data));
}

PyObjPtr Ones(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Ones(): args is not a list");
  }
  auto argList = args->as<PyList>();

  if (argList->Length() != 1) {
    throw std::runtime_error("Ones(): args length is not 1");
  }
  auto shape = argList->GetItem(0)->as<PyList>();
  if (shape->Length() != 2) {
    throw std::runtime_error("Ones(): shape length is not 2");
  }
  auto rows = shape->GetItem(0)->as<PyInteger>()->ToU64();
  auto cols = shape->GetItem(1)->as<PyInteger>()->ToU64();
  Collections::List<double> data(rows * cols, 1.0);
  return CreatePyMatrix(Collections::Matrix(rows, cols, data));
}

PyObjPtr Diagnostic(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Diagnostic(): args is not a list");
  }
  auto argList = args->as<PyList>();

  if (argList->Length() != 1) {
    throw std::runtime_error("Diagnostic(): args length is not 1");
  }
  auto arg0 = argList->GetItem(0);
  if (!arg0->is(ListKlass::Self())) {
    throw std::runtime_error("Diagnostic(): arg0 is not a list");
  }
  auto list = arg0->as<PyList>();
  auto dim = list->Length();
  Collections::List<double> data(dim * dim, 0.0);
  for (Index i = 0; i < dim; i++) {
    auto element = list->GetItem(i);
    if (!element->is(FloatKlass::Self())) {
      throw std::runtime_error("Diagnostic(): element is not a float");
    }
    data[(i * dim) + i] = element->as<PyFloat>()->Value();
  }
  return CreatePyMatrix(Collections::Matrix(dim, dim, data));
}

PyObjPtr Shape(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Shape(): args is not a list");
  }
  return args->as<PyList>()->GetItem(0)->as<PyMatrix>()->Shape();
}

PyObjPtr Reshape(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Reshape(): args is not a list");
  }
  auto argList = args->as<PyList>();

  if (argList->Length() != 2) {
    throw std::runtime_error("Reshape(): args length is not 2");
  }
  auto matrix = argList->GetItem(0)->as<PyMatrix>();
  auto list = argList->GetItem(1)->as<PyList>();
  if (list->Length() != 2) {
    throw std::runtime_error("Reshape(): list length is not 2");
  }
  auto rows = list->GetItem(0)->as<PyInteger>()->ToU64();
  auto cols = list->GetItem(1)->as<PyInteger>()->ToU64();
  return matrix->Reshape(rows, cols);
}

PyObjPtr Ravel(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Ravel(): args is not a list");
  }
  auto argList = args->as<PyList>();

  if (argList->Length() != 1) {
    throw std::runtime_error("Ravel(): args length is not 1");
  }
  auto matrix = argList->GetItem(0)->as<PyMatrix>();
  auto data = matrix->Ravel();
  Collections::List<PyObjPtr> result(data.Size());
  for (Index i = 0; i < data.Size(); i++) {
    result.Push(CreatePyFloat(data[i]));
  }
  return PyList::Create(result);
}

PyObjPtr Concatenate(const PyObjPtr& args) {
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("Concatenate(): args is not a list");
  }
  auto argList = args->as<PyList>();
  if (argList->Length() != 1) {
    throw std::runtime_error("Concatenate(): args length is not 1");
  }
  auto matrixList = argList->GetItem(0)->as<PyList>();
  auto firstMatrix = matrixList->GetItem(0)->as<PyMatrix>();
  auto cols = firstMatrix->Shape()->GetItem(1)->as<PyInteger>()->ToU64();
  Collections::List<double> data;
  for (Index i = 0; i < matrixList->Length(); i++) {
    auto matrix = matrixList->GetItem(i)->as<PyMatrix>();
    if (matrix->Shape()->GetItem(1)->as<PyInteger>()->ToU64() != cols) {
      throw std::runtime_error("Concatenate(): cols is not equal");
    }
    data.Concat(matrix->Ravel());
  }
  auto rows = data.Size() / cols;
  return CreatePyMatrix(Collections::Matrix(rows, cols, data));
}

PyObjPtr Transpose(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  CheckNativeFunctionArgumentsAtIndexWithType(
    PyString::Create("Transpose")->as<PyString>(), args, 0, MatrixKlass::Self()
  );
  return args->as<PyList>()->GetItem(0)->as<PyMatrix>()->Transpose();
}
}  // namespace kaubo::Object