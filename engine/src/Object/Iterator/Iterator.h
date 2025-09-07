#pragma once

#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
namespace kaubo::Object {

class IterDoneKlass : public KlassBase<IterDoneKlass> {
 public:
  explicit IterDoneKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("StopIteration")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }
};

class IterDone : public PyObject {
 public:
  explicit IterDone() : PyObject(IterDoneKlass::Self()) {}
};

class ListIteratorKlass : public KlassBase<ListIteratorKlass> {
 public:
  explicit ListIteratorKlass() = default;

  PyObjPtr iter(const PyObjPtr& obj) override { return obj; }
  PyObjPtr next(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override { return repr(obj); }
  PyObjPtr repr(const PyObjPtr& obj) override;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("ListIterator")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }
};

class ListReverseIteratorKlass : public KlassBase<ListReverseIteratorKlass> {
 public:
  explicit ListReverseIteratorKlass() = default;

  PyObjPtr iter(const PyObjPtr& obj) override { return obj; }
  PyObjPtr next(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override { return repr(obj); }
  PyObjPtr repr(const PyObjPtr& obj) override;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("ListReverseIterator")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }
};

inline PyObjPtr CreateIterDone() {
  return std::make_shared<IterDone>();
}

class ListIterator : public PyObject {
 private:
  PyListPtr list;
  Index index{};

 public:
  explicit ListIterator(const PyObjPtr& list)
    : PyObject(ListIteratorKlass::Self()) {
    this->list = list->as<PyList>();
  }
  [[nodiscard]] PyListPtr List() const { return list; }
  [[nodiscard]] Index CurrentIndex() const { return index; }
  void Next() { index++; }
};

inline PyObjPtr CreateListIterator(const PyObjPtr& list) {
  return std::make_shared<ListIterator>(list);
}

class ListReverseIterator : public PyObject {
 private:
  PyListPtr list;
  Index index{};

 public:
  explicit ListReverseIterator(const PyObjPtr& list)
    : PyObject(ListReverseIteratorKlass::Self()),
      list(list->as<PyList>()),
      index(this->list->Length() - 1) {}

  [[nodiscard]] PyListPtr List() const { return list; }
  [[nodiscard]] Index CurrentIndex() const { return index; }
  void Next() { index--; }
};

class StringIteratorKlass : public KlassBase<StringIteratorKlass> {
 public:
  explicit StringIteratorKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("StringIterator")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }
  PyObjPtr iter(const PyObjPtr& obj) override { return obj; }
  PyObjPtr next(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override;
};

inline PyObjPtr CreateListReverseIterator(const PyObjPtr& list) {
  return std::make_shared<ListReverseIterator>(list);
}

class StringIterator : public PyObject {
 private:
  PyStrPtr string;
  Index index{};

 public:
  explicit StringIterator(const PyObjPtr& string)
    : PyObject(StringIteratorKlass::Self()) {
    this->string = string->as<PyString>();
  }
  [[nodiscard]] PyStrPtr String() const { return string; }
  [[nodiscard]] Index CurrentIndex() const { return index; }
  void Next() { index++; }
};

inline PyObjPtr CreateStringIterator(const PyObjPtr& string) {
  return std::make_shared<StringIterator>(string);
}

class DictItemIteratorKlass : public KlassBase<DictItemIteratorKlass> {
 public:
  explicit DictItemIteratorKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(CreatePyString("DictItemIterator")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }
  PyObjPtr iter(const PyObjPtr& obj) override { return obj; }
  PyObjPtr next(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override;
};

class DictItemIterator : public PyObject {
 private:
  PyDictPtr dict;
  Index index{};

 public:
  explicit DictItemIterator(PyDictPtr dict)
    : PyObject(DictItemIteratorKlass::Self()), dict(std::move(dict)) {}
  [[nodiscard]] PyDictPtr Dict() const { return dict; }
  [[nodiscard]] Index CurrentIndex() const { return index; }
  void Next() { index++; }
};

inline PyObjPtr CreateDictItemIterator(const PyDictPtr& dict) {
  return std::make_shared<DictItemIterator>(dict);
}

}  // namespace kaubo::Object
