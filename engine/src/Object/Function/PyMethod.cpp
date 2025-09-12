#include "Object/Function/PyMethod.h"
#include "Object/Container/PyList.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr MethodKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(MethodKlass::Self())) {
    auto errorMessage = StringConcat(CreatePyList(
      {PyString::Create("AttributeError: '")->as<PyString>(),
       obj->Klass()->Name(),
       PyString::Create("' object has no attribute '__repr__'")->as<PyString>()}
    ));
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  auto method = obj->as<PyMethod>();
  return StringConcat(CreatePyList(
    {PyString::Create("<method "), method->Owner()->repr(),
     PyString::Create(".")->as<PyString>(), method->Method()->repr(),
     PyString::Create(">")->as<PyString>()}
  ));
}

}  // namespace kaubo::Object