#include "Runtime/Genesis.h"
#include "Function/BuiltinFunction.h"
#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Core/PyPromise.h"

#include "Object/Function/PyNativeFunction.h"
#include "Object/Matrix/MatrixFunction.h"
#include "Object/Number/PyFloat.h"
#include "Object/Number/PyInteger.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {
class PyType : public PyObject {};
}  // namespace kaubo::Object

namespace kaubo::Runtime {

Object::PyDictPtr Genesis() {
  Object::LoadBootstrapClasses();
  Object::LoadRuntimeSupportClasses();
  auto builtins = Object::PyDictionary::Create();
  // 注册内置函数和类型
  builtins->Put(Object::PyString::Create("None"), Object::PyNone::Create());
  builtins->Put(
    Object::PyString::Create("True"), Object::PyBoolean::Create(true)
  );
  builtins->Put(
    Object::PyString::Create("False"), Object::PyBoolean::Create(false)
  );

  // 注册内置函数
  builtins->Put(
    Object::PyString::Create("print"),
    Object::PyNativeFunction::Create(Function::Print)
  );
  builtins->Put(
    Object::PyString::Create("readFile"),
    Object::PyNativeFunction::Create(Function::ReadFile)
  );
  builtins->Put(
    Object::PyString::Create("len"),
    Object::PyNativeFunction::Create(Function::Len)
  );
  builtins->Put(
    Object::PyString::Create("next"),
    Object::PyNativeFunction::Create(Function::Next)
  );
  builtins->Put(
    Object::PyString::Create("iter"),
    Object::PyNativeFunction::Create(Function::Iter)
  );
  builtins->Put(
    Object::PyString::Create("str"),
    Object::PyNativeFunction::Create(Object::Str)
  );
  builtins->Put(
    Object::PyString::Create("repr"),
    Object::PyNativeFunction::Create(Object::Repr)
  );
  builtins->Put(
    Object::PyString::Create("bool"),
    Object::PyNativeFunction::Create(Object::Bool)
  );
  builtins->Put(
    Object::PyString::Create("id"),
    Object::PyNativeFunction::Create(Function::Identity)
  );
  builtins->Put(
    Object::PyString::Create("hash"),
    Object::PyNativeFunction::Create(Function::Hash)
  );
  builtins->Put(
    Object::PyString::Create("time"),
    Object::PyNativeFunction::Create(Function::Time)
  );
  builtins->Put(
    Object::PyString::Create("range"),
    Object::PyNativeFunction::Create(Function::Range)
  );
  builtins->Put(
    Object::PyString::Create("__build_class__"),
    Object::PyNativeFunction::Create(Function::BuildClass)
  );
  builtins->Put(
    Object::PyString::Create("type"),
    Object::PyNativeFunction::Create(Function::Type)
  );
  builtins->Put(
    Object::PyString::Create("whoami"),
    Object::PyString::Create("版权所有 © 2025 蒋钦禹. 保留所有权利。")
  );
  // builtins->Put(
  //   Object::PyString::Create("co"),
  //   Object::PyNativeFunction::Create(Function::Coroutine)
  // );

  // 内置全局对象
  builtins->Put(
    Object::PyString::Create("Array"),
    Object::PyNativeFunction::Create(Object::Array)
  );
  builtins->Put(
    Object::PyString::Create("Eye"),
    Object::PyNativeFunction::Create(Object::Eye)
  );
  builtins->Put(
    Object::PyString::Create("Zeros"),
    Object::PyNativeFunction::Create(Object::Zeros)
  );
  builtins->Put(
    Object::PyString::Create("Ones"),
    Object::PyNativeFunction::Create(Object::Ones)
  );
  builtins->Put(
    Object::PyString::Create("Diag"),
    Object::PyNativeFunction::Create(Object::Diagnostic)
  );
  builtins->Put(
    Object::PyString::Create("Transpose"),
    Object::PyNativeFunction::Create(Object::Transpose)
  );
  builtins->Put(
    Object::PyString::Create("Reshape"),
    Object::PyNativeFunction::Create(Object::Reshape)
  );
  builtins->Put(
    Object::PyString::Create("Shape"),
    Object::PyNativeFunction::Create(Object::Shape)
  );
  builtins->Put(
    Object::PyString::Create("Concatenate"),
    Object::PyNativeFunction::Create(Object::Concatenate)
  );
  builtins->Put(
    Object::PyString::Create("Ravel"),
    Object::PyNativeFunction::Create(Object::Ravel)
  );
  builtins->Put(
    Object::PyString::Create("Normal"),
    Object::PyNativeFunction::Create(Function::Normal)
  );
  builtins->Put(
    Object::PyString::Create("Shuffle"),
    Object::PyNativeFunction::Create(Function::Shuffle)
  );
  builtins->Put(
    Object::PyString::Create("LogisticLoss"),
    Object::PyNativeFunction::Create(Function::LogisticLoss)
  );
  builtins->Put(
    Object::PyString::Create("LogisticLossDerivative"),
    Object::PyNativeFunction::Create(Function::LogisticLossDerivative)
  );
  builtins->Put(
    Object::PyString::Create("Sum"),
    Object::PyNativeFunction::Create(Function::Sum)
  );
  builtins->Put(
    Object::PyString::Create("Log"),
    Object::PyNativeFunction::Create(Function::Log)
  );
  builtins->Put(
    Object::PyString::Create("Softmax"),
    Object::PyNativeFunction::Create(Function::SoftMax)
  );
  builtins->Put(
    Object::PyString::Create("Max"),
    Object::PyNativeFunction::Create(Function::Max)
  );
  builtins->Put(
    Object::PyString::Create("ArgMax"),
    Object::PyNativeFunction::Create(Function::ArgMax)
  );

  // 系统相关函数
  builtins->Put(
    Object::PyString::Create("input"),
    Object::PyNativeFunction::Create(Function::Input)
  );
  builtins->Put(
    Object::PyString::Create("sleep"),
    Object::PyNativeFunction::Create(Function::Sleep)
  );
  builtins->Put(
    Object::PyString::Create("randint"),
    Object::PyNativeFunction::Create(Function::RandInt)
  );

  // 注册切片类型
  builtins->Put(
    Object::PyString::Create("slice"), Object::SliceKlass::Self()->Type()
  );

  // 注册内置类型
  builtins->Put(
    Object::PyString::Create("int"), Object::IntegerKlass::Self()->Type()
  );
  builtins->Put(
    Object::PyString::Create("float"), Object::FloatKlass::Self()->Type()
  );
  builtins->Put(
    Object::PyString::Create("list"), Object::ListKlass::Self()->Type()

  );
  builtins->Put(
    Object::PyString::Create("dict"), Object::DictionaryKlass::Self()->Type()
  );
  builtins->Put(
    Object::PyString::Create("Promise"), Object::PromiseKlass::Self()->Type()
  );
  builtins->Put(
    Object::PyString::Create("object"), Object::ObjectKlass::Self()->Type()
  );
  return builtins;
}

}  // namespace kaubo::Runtime
