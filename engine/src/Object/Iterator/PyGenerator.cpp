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
  InitKlass(CreatePyString("generator"), GeneratorKlass::Self());
  AddAttribute(
    CreatePyString("__next__"),
    CreatePyNativeFunction(
      CreateForwardFunction<GeneratorKlass>(&GeneratorKlass::next)
    )
  );
  AddAttribute(
    CreatePyString("done"), CreatePyIife([](const PyObjPtr& args) {
      return CreatePyBoolean(
        args->as<PyList>()->GetItem(0)->as<PyGenerator>()->IsExhausted()
      );
    })
  );
  AddAttribute(
    CreatePyString("send"), CreatePyNativeFunction([](const PyObjPtr& args) {
      auto argList = args->as<PyList>();
      auto generator = argList->GetItem(0)->as<PyGenerator>();
      auto value = argList->GetItem(1);
      return generator->Send(value);
    })
  );
  this->SetInitialized();
}

}  // namespace kaubo::Object