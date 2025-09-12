#include "Object/Function/PyFunction.h"
#include "Function/BuiltinFunction.h"

namespace kaubo::Object {

PyObjPtr FunctionKlass::repr(const PyObjPtr& obj) {
  return StringConcat(CreatePyList(
    {PyString::Create("<function ")->as<PyString>(),
     obj->as<PyFunction>()->Name(), PyString::Create(" at ")->as<PyString>(),
     Function::Identity(CreatePyList({obj}))->as<PyString>(),
     PyString::Create(">")->as<PyString>()}
  ));
}

}  // namespace kaubo::Object