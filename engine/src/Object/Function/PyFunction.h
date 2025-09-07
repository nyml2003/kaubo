#pragma once
#include "Object/Container/PyDictionary.h"
#include "Object/Core/PyObject.h"
#include "Object/Runtime/PyCode.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class FunctionKlass : public KlassBase<FunctionKlass> {
 public:
  FunctionKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("function")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr repr(const PyObjPtr& obj) override;
};

class PyFunction : public PyObject {
 private:
  PyCodePtr code;
  PyDictPtr globals;

 public:
  PyFunction(const PyObjPtr& code, const PyObjPtr& globals)
    : PyObject(FunctionKlass::Self()),
      code(code->as<PyCode>()),
      globals(globals->as<PyDictionary>()) {}

  [[nodiscard]] PyCodePtr Code() const { return code; }

  [[nodiscard]] PyDictPtr Globals() const { return globals; }

  [[nodiscard]] PyStrPtr Name() const { return code->Name(); }
};

using PyFunctionPtr = std::shared_ptr<PyFunction>;

inline PyObjPtr
CreatePyFunction(const PyObjPtr& code, const PyObjPtr& globals) {
  return std::make_shared<PyFunction>(code, globals);
}

}  // namespace kaubo::Object
