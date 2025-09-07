#include "Object/Function/PyMethod.h"
#include "Object/Container/PyList.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr MethodKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(MethodKlass::Self())) {
    auto errorMessage = StringConcat(CreatePyList(
      {CreatePyString("AttributeError: '")->as<PyString>(),
       obj->Klass()->Name(),
       CreatePyString("' object has no attribute '__repr__'")->as<PyString>()}
    ));
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  auto method = obj->as<PyMethod>();
  return StringConcat(CreatePyList(
    {CreatePyString("<method "), method->Owner()->repr(),
     CreatePyString(".")->as<PyString>(), method->Method()->repr(),
     CreatePyString(">")->as<PyString>()}
  ));
}

}  // namespace kaubo::Object