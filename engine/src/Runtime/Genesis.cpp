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
  builtins->Put(Object::PyString::Create("None"), Object::CreatePyNone());
  builtins->Put(
    Object::PyString::Create("True"), Object::PyBoolean::Create(true)
  );
  builtins->Put(
    Object::PyString::Create("False"), Object::PyBoolean::Create(false)
  );

  // 注册内置函数
  builtins->Put(
    Object::PyString::Create("print"), CreatePyNativeFunction(Function::Print)
  );
  builtins->Put(
    Object::PyString::Create("readFile"),
    CreatePyNativeFunction(Function::ReadFile)
  );
  builtins->Put(
    Object::PyString::Create("len"), CreatePyNativeFunction(Function::Len)
  );
  builtins->Put(
    Object::PyString::Create("next"), CreatePyNativeFunction(Function::Next)
  );
  builtins->Put(
    Object::PyString::Create("iter"), CreatePyNativeFunction(Function::Iter)
  );
  builtins->Put(
    Object::PyString::Create("str"), CreatePyNativeFunction(Object::Str)
  );
  builtins->Put(
    Object::PyString::Create("repr"), CreatePyNativeFunction(Object::Repr)
  );
  builtins->Put(
    Object::PyString::Create("bool"), CreatePyNativeFunction(Object::Bool)
  );
  builtins->Put(
    Object::PyString::Create("id"), CreatePyNativeFunction(Function::Identity)
  );
  builtins->Put(
    Object::PyString::Create("hash"), CreatePyNativeFunction(Function::Hash)
  );
  builtins->Put(
    Object::PyString::Create("time"), CreatePyNativeFunction(Function::Time)
  );
  builtins->Put(
    Object::PyString::Create("range"), CreatePyNativeFunction(Function::Range)
  );
  builtins->Put(
    Object::PyString::Create("__build_class__"),
    CreatePyNativeFunction(Function::BuildClass)
  );
  builtins->Put(
    Object::PyString::Create("type"), CreatePyNativeFunction(Function::Type)
  );
  builtins->Put(
    Object::PyString::Create("whoami"),
    Object::PyString::Create("版权所有 © 2025 蒋钦禹. 保留所有权利。")
  );
  // builtins->Put(
  //   Object::PyString::Create("co"),
  //   Object::CreatePyNativeFunction(Function::Coroutine)
  // );

  // 内置全局对象
  builtins->Put(
    Object::PyString::Create("Array"),
    Object::CreatePyNativeFunction(Object::Array)
  );
  builtins->Put(
    Object::PyString::Create("Eye"), Object::CreatePyNativeFunction(Object::Eye)
  );
  builtins->Put(
    Object::PyString::Create("Zeros"),
    Object::CreatePyNativeFunction(Object::Zeros)
  );
  builtins->Put(
    Object::PyString::Create("Ones"),
    Object::CreatePyNativeFunction(Object::Ones)
  );
  builtins->Put(
    Object::PyString::Create("Diag"),
    Object::CreatePyNativeFunction(Object::Diagnostic)
  );
  builtins->Put(
    Object::PyString::Create("Transpose"),
    Object::CreatePyNativeFunction(Object::Transpose)
  );
  builtins->Put(
    Object::PyString::Create("Reshape"),
    Object::CreatePyNativeFunction(Object::Reshape)
  );
  builtins->Put(
    Object::PyString::Create("Shape"),
    Object::CreatePyNativeFunction(Object::Shape)
  );
  builtins->Put(
    Object::PyString::Create("Concatenate"),
    Object::CreatePyNativeFunction(Object::Concatenate)
  );
  builtins->Put(
    Object::PyString::Create("Ravel"),
    Object::CreatePyNativeFunction(Object::Ravel)
  );
  builtins->Put(
    Object::PyString::Create("Normal"),
    Object::CreatePyNativeFunction(Function::Normal)
  );
  builtins->Put(
    Object::PyString::Create("Shuffle"),
    Object::CreatePyNativeFunction(Function::Shuffle)
  );
  builtins->Put(
    Object::PyString::Create("LogisticLoss"),
    Object::CreatePyNativeFunction(Function::LogisticLoss)
  );
  builtins->Put(
    Object::PyString::Create("LogisticLossDerivative"),
    Object::CreatePyNativeFunction(Function::LogisticLossDerivative)
  );
  builtins->Put(
    Object::PyString::Create("Sum"),
    Object::CreatePyNativeFunction(Function::Sum)
  );
  builtins->Put(
    Object::PyString::Create("Log"),
    Object::CreatePyNativeFunction(Function::Log)
  );
  builtins->Put(
    Object::PyString::Create("Softmax"),
    Object::CreatePyNativeFunction(Function::SoftMax)
  );
  builtins->Put(
    Object::PyString::Create("Max"),
    Object::CreatePyNativeFunction(Function::Max)
  );
  builtins->Put(
    Object::PyString::Create("ArgMax"),
    Object::CreatePyNativeFunction(Function::ArgMax)
  );

  // 系统相关函数
  builtins->Put(
    Object::PyString::Create("input"), CreatePyNativeFunction(Function::Input)
  );
  builtins->Put(
    Object::PyString::Create("sleep"), CreatePyNativeFunction(Function::Sleep)
  );
  builtins->Put(
    Object::PyString::Create("randint"),
    CreatePyNativeFunction(Function::RandInt)
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
