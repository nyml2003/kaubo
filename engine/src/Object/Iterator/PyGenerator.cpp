#include "Object/Iterator/PyGenerator.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Function/FunctionForward.h"
#include "Object/Function/PyIife.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr GeneratorKlass::next(const PyObjPtr& obj) {
  return obj->as<PyGenerator>()->Next();
}

void GeneratorKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  InitKlass(PyString::Create("generator"), GeneratorKlass::Self());
  AddAttribute(
    PyString::Create("__next__"),
    CreatePyNativeFunction(
      CreateForwardFunction<GeneratorKlass>(&GeneratorKlass::next)
    )
  );
  AddAttribute(
    PyString::Create("done"), CreatePyIife([](const PyObjPtr& args) {
      return PyBoolean::Create(
        args->as<PyList>()->GetItem(0)->as<PyGenerator>()->IsExhausted()
      );
    })
  );
  AddAttribute(
    PyString::Create("send"), CreatePyNativeFunction([](const PyObjPtr& args) {
      auto argList = args->as<PyList>();
      auto generator = argList->GetItem(0)->as<PyGenerator>();
      auto value = argList->GetItem(1);
      return generator->Send(value);
    })
  );
  this->SetInitialized();
}

}  // namespace kaubo::Object