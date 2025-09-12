#pragma once

#include "Object/Core/CoreHelper.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

class BytesKlass : public KlassBase<BytesKlass> {
 public:
  explicit BytesKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(PyString::Create("bytes")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr _serialize_(const PyObjPtr& obj) override;

  PyObjPtr repr(const PyObjPtr& obj) override;
};
class PyBytes;
using PyBytesPtr = std::shared_ptr<PyBytes>;
class PyBytes : public PyObject, public IObjectCreator<PyBytes> {
 private:
  Collections::String value;

 public:
  explicit PyBytes(Collections::String&& value)
    : PyObject(BytesKlass::Self()), value(std::move(value)) {}

  [[nodiscard]] const Collections::String& Value() { return value; }
};

}  // namespace kaubo::Object
