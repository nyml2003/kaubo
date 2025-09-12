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
  auto builtins = Object::CreatePyDict()->as<Object::PyDictionary>();
  // 注册内置函数和类型
  builtins->Put(Object::CreatePyString("None"), Object::CreatePyNone());
  builtins->Put(
    Object::CreatePyString("True"), Object::PyBoolean::create(true)
  );
  builtins->Put(
    Object::CreatePyString("False"), Object::PyBoolean::create(false)
  );

  // 注册内置函数
  builtins->Put(
    Object::CreatePyString("print"), CreatePyNativeFunction(Function::Print)
  );
  builtins->Put(
    Object::CreatePyString("readFile"),
    CreatePyNativeFunction(Function::ReadFile)
  );
  builtins->Put(
    Object::CreatePyString("len"), CreatePyNativeFunction(Function::Len)
  );
  builtins->Put(
    Object::CreatePyString("next"), CreatePyNativeFunction(Function::Next)
  );
  builtins->Put(
    Object::CreatePyString("iter"), CreatePyNativeFunction(Function::Iter)
  );
  builtins->Put(
    Object::CreatePyString("str"), CreatePyNativeFunction(Object::Str)
  );
  builtins->Put(
    Object::CreatePyString("repr"), CreatePyNativeFunction(Object::Repr)
  );
  builtins->Put(
    Object::CreatePyString("bool"), CreatePyNativeFunction(Object::Bool)
  );
  builtins->Put(
    Object::CreatePyString("id"), CreatePyNativeFunction(Function::Identity)
  );
  builtins->Put(
    Object::CreatePyString("hash"), CreatePyNativeFunction(Function::Hash)
  );
  builtins->Put(
    Object::CreatePyString("time"), CreatePyNativeFunction(Function::Time)
  );
  builtins->Put(
    Object::CreatePyString("range"), CreatePyNativeFunction(Function::Range)
  );
  builtins->Put(
    Object::CreatePyString("__build_class__"),
    CreatePyNativeFunction(Function::BuildClass)
  );
  builtins->Put(
    Object::CreatePyString("type"), CreatePyNativeFunction(Function::Type)
  );
  builtins->Put(
    Object::CreatePyString("whoami"),
    Object::CreatePyString("版权所有 © 2025 蒋钦禹. 保留所有权利。")
  );
  // builtins->Put(
  //   Object::CreatePyString("co"),
  //   Object::CreatePyNativeFunction(Function::Coroutine)
  // );

  // 内置全局对象
  builtins->Put(
    Object::CreatePyString("Array"),
    Object::CreatePyNativeFunction(Object::Array)
  );
  builtins->Put(
    Object::CreatePyString("Eye"), Object::CreatePyNativeFunction(Object::Eye)
  );
  builtins->Put(
    Object::CreatePyString("Zeros"),
    Object::CreatePyNativeFunction(Object::Zeros)
  );
  builtins->Put(
    Object::CreatePyString("Ones"), Object::CreatePyNativeFunction(Object::Ones)
  );
  builtins->Put(
    Object::CreatePyString("Diag"),
    Object::CreatePyNativeFunction(Object::Diagnostic)
  );
  builtins->Put(
    Object::CreatePyString("Transpose"),
    Object::CreatePyNativeFunction(Object::Transpose)
  );
  builtins->Put(
    Object::CreatePyString("Reshape"),
    Object::CreatePyNativeFunction(Object::Reshape)
  );
  builtins->Put(
    Object::CreatePyString("Shape"),
    Object::CreatePyNativeFunction(Object::Shape)
  );
  builtins->Put(
    Object::CreatePyString("Concatenate"),
    Object::CreatePyNativeFunction(Object::Concatenate)
  );
  builtins->Put(
    Object::CreatePyString("Ravel"),
    Object::CreatePyNativeFunction(Object::Ravel)
  );
  builtins->Put(
    Object::CreatePyString("Normal"),
    Object::CreatePyNativeFunction(Function::Normal)
  );
  builtins->Put(
    Object::CreatePyString("Shuffle"),
    Object::CreatePyNativeFunction(Function::Shuffle)
  );
  builtins->Put(
    Object::CreatePyString("LogisticLoss"),
    Object::CreatePyNativeFunction(Function::LogisticLoss)
  );
  builtins->Put(
    Object::CreatePyString("LogisticLossDerivative"),
    Object::CreatePyNativeFunction(Function::LogisticLossDerivative)
  );
  builtins->Put(
    Object::CreatePyString("Sum"), Object::CreatePyNativeFunction(Function::Sum)
  );
  builtins->Put(
    Object::CreatePyString("Log"), Object::CreatePyNativeFunction(Function::Log)
  );
  builtins->Put(
    Object::CreatePyString("Softmax"),
    Object::CreatePyNativeFunction(Function::SoftMax)
  );
  builtins->Put(
    Object::CreatePyString("Max"), Object::CreatePyNativeFunction(Function::Max)
  );
  builtins->Put(
    Object::CreatePyString("ArgMax"),
    Object::CreatePyNativeFunction(Function::ArgMax)
  );

  // 系统相关函数
  builtins->Put(
    Object::CreatePyString("input"), CreatePyNativeFunction(Function::Input)
  );
  builtins->Put(
    Object::CreatePyString("sleep"), CreatePyNativeFunction(Function::Sleep)
  );
  builtins->Put(
    Object::CreatePyString("randint"), CreatePyNativeFunction(Function::RandInt)
  );

  // 注册切片类型
  builtins->Put(
    Object::CreatePyString("slice"), Object::SliceKlass::Self()->Type()
  );

  // 注册内置类型
  builtins->Put(
    Object::CreatePyString("int"), Object::IntegerKlass::Self()->Type()
  );
  builtins->Put(
    Object::CreatePyString("float"), Object::FloatKlass::Self()->Type()
  );
  builtins->Put(
    Object::CreatePyString("list"), Object::ListKlass::Self()->Type()

  );
  builtins->Put(
    Object::CreatePyString("dict"), Object::DictionaryKlass::Self()->Type()
  );
  builtins->Put(
    Object::CreatePyString("Promise"), Object::PromiseKlass::Self()->Type()
  );
  builtins->Put(
    Object::CreatePyString("object"), Object::ObjectKlass::Self()->Type()
  );
  return builtins;
}

}  // namespace kaubo::Runtime
