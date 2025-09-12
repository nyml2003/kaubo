#include "Object/Core/PyObject.h"
#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Runtime/VirtualMachine.h"

namespace kaubo::Object {

PyDictPtr PyObject::Attributes() noexcept {
  if (attributes == nullptr) {
    attributes = PyDictionary::Create();
  }
  return attributes;
}

PyDictPtr PyObject::Methods() noexcept {
  if (methods == nullptr) {
    methods = PyDictionary::Create();
  }
  return methods;
}

PyObjPtr ObjectInit(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto self = args->as<PyList>()->GetItem(0);
  auto* klass = self->Klass();
  auto instance = std::make_shared<PyObject>(klass);
  return instance;
}

void ObjectKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  auto* instance = Self();
  instance->SetName(PyString::Create("object")->as<PyString>());
  instance->SetAttributes(PyDictionary::Create());
  instance->AddAttribute(
    PyString::Create("__init__")->as<PyString>(),
    CreatePyNativeFunction(ObjectInit)
  );
  instance->AddAttribute(
    PyString::Create("__str__")->as<PyString>(),
    CreatePyNativeFunction(KlassStr)
  );
  instance->AddAttribute(
    PyString::Create("__repr__")->as<PyString>(),
    CreatePyNativeFunction(KlassRepr)
  );
  instance->AddAttribute(
    PyString::Create("__bool__")->as<PyString>(),
    CreatePyNativeFunction(KlassBool)
  );
  instance->SetType(CreatePyType(instance)->as<PyType>());
  instance->SetSuper(CreatePyList()->as<PyList>());
  instance->SetMro(
    CreatePyList({CreatePyType(instance)->as<PyType>()})->as<PyList>()
  );
  instance->SetNative();
  ConfigureBasicAttributes(instance);
  this->SetInitialized();
}

bool operator==(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (lhs.get() == rhs.get()) {
    return true;
  }
  if (lhs->Klass() != rhs->Klass()) {
    return false;
  }
  auto equal = lhs->eq(rhs);
  return equal->as<PyBoolean>()->Value();
}

bool operator!=(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  return !(lhs == rhs);
}

bool operator<(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  auto ltFunc = lhs->getattr(PyString::Create("__lt__")->as<PyString>());
  if (ltFunc != nullptr) {
    return Runtime::Evaluator::InvokeCallable(
             ltFunc, CreatePyList({rhs})->as<PyList>()
    )
      ->as<PyBoolean>()
      ->Value();
  }
  throw std::runtime_error("TypeError: '<' not supported between instances");
}

}  // namespace kaubo::Object