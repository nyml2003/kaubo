#include "Object/Function/PyIife.h"
#include "Object/Core/CoreHelper.h"
#include "Object/String/PyString.h"
namespace kaubo::Object {

void IifeKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  InitKlass(CreatePyString("iife")->as<PyString>(), IifeKlass::Self());
  ConfigureBasicAttributes(IifeKlass::Self());
  this->SetInitialized();
}

PyObjPtr IifeKlass::str(const PyObjPtr& /*self*/) {
  return CreatePyString("<iife>");
}

}  // namespace kaubo::Object