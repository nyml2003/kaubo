#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Iterator/Iterator.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Number/PyInteger.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

void PyDictionary::Put(const PyObjPtr& key, const PyObjPtr& value) {
  dict.insert_or_assign(key, value);
}

PyObjPtr PyDictionary::Get(const PyObjPtr& key) {
  return dict[key];
}

PyObjPtr PyDictionary::TryGet(const PyObjPtr& key) const {
  auto dictIter = dict.find(key);
  if (dictIter != dict.end()) {
    return dictIter->second;
  }
  return nullptr;  // 表示没有找到
}

void PyDictionary::Remove(const PyObjPtr& key) {
  dict.erase(key);
}

bool PyDictionary::Contains(const PyObjPtr& key) {
  return dict.find(key) != dict.end();
}

PyObjPtr PyDictionary::GetItem(Index index) const {
  auto dictIterator = dict.begin();
  std::advance(dictIterator, index);
  auto entry = *dictIterator;
  return CreatePyList({entry.first, entry.second});
}

PyDictPtr PyDictionary::Add(const PyDictPtr& other) {
  auto newDict = PyDictionary::Create();
  for (const auto& item : dict) {
    newDict->Put(item.first, item.second);
  }
  for (const auto& item : other->dict) {
    newDict->Put(item.first, item.second);
  }
  return newDict;
}

Index PyDictionary::Size() const {
  return dict.size();
}

void DictionaryKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  InitKlass(PyString::Create("dict")->as<PyString>(), Self());
  ConfigureBasicAttributes(Self());
  Self()->AddAttribute(
    PyString::Create("clear")->as<PyString>(),
    CreatePyNativeFunction(DictClear)->as<PyNativeFunction>()
  );
  Self()->AddAttribute(
    PyString::Create("items")->as<PyString>(),
    CreatePyNativeFunction(DictItems)->as<PyNativeFunction>()
  );
  Self()->AddAttribute(
    PyString::Create("get")->as<PyString>(),
    CreatePyNativeFunction(DictGet)->as<PyNativeFunction>()
  );

  this->SetInitialized();
}

PyObjPtr DictionaryKlass::init(const PyObjPtr& klass, const PyObjPtr& args) {
  if (klass->as<PyType>()->Owner() != Self()) {
    throw std::runtime_error("PyDictionary::init(): klass is not a dict");
  }
  auto argList = args->as<PyList>();
  if (argList->Length() != 0) {
    throw std::runtime_error("PyDictionary::init(): args must be empty");
  }
  return PyDictionary::Create();
}

PyObjPtr DictionaryKlass::setitem(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::setitem(): obj is not a dict");
  }
  auto dict = obj->as<PyDictionary>();
  dict->Put(key, value);
  return CreatePyNone();
}

PyObjPtr DictionaryKlass::getitem(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::getitem(): obj is not a dict");
  }
  auto dict = obj->as<PyDictionary>();
  return dict->Get(key);
}

PyObjPtr DictionaryKlass::delitem(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::delitem(): obj is not a dict");
  }
  auto dict = obj->as<PyDictionary>();
  dict->Remove(key);
  return obj;
}

PyObjPtr DictionaryKlass::iter(const PyObjPtr& obj) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::iter(): obj is not a dict");
  }
  return CreateDictItemIterator(obj->as<PyDictionary>());
}

PyObjPtr DictionaryKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::repr(): obj is not a dict");
  }
  auto dictItemReprList = Map(obj, [](const PyObjPtr& item) {
    auto key = item->as<PyList>()->GetItem(0);
    auto value = item->as<PyList>()->GetItem(1);
    return StringConcat(CreatePyList(
      {key->repr(), PyString::Create(": ")->as<PyString>(), value->repr()}
    ));
  });
  auto repr = PyString::Create(", ")->as<PyString>()->Join(dictItemReprList);
  return StringConcat(CreatePyList(
    {PyString::Create("{")->as<PyString>(), repr,
     PyString::Create("}")->as<PyString>()}
  ));
}

PyObjPtr DictionaryKlass::str(const PyObjPtr& obj) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::str(): obj is not a dict");
  }
  auto dictItemStrList = Map(obj, [](const PyObjPtr& item) {
    auto key = item->as<PyList>()->GetItem(0);
    auto value = item->as<PyList>()->GetItem(1);
    return StringConcat(CreatePyList(
      {key->str()->repr(), PyString::Create(": ")->as<PyString>(), value->str()}
    ));
  });
  auto repr = PyString::Create(", ")->as<PyString>()->Join(dictItemStrList);
  return StringConcat(CreatePyList(
    {PyString::Create("{")->as<PyString>(), repr,
     PyString::Create("}")->as<PyString>()}
  ));
}

PyObjPtr DictionaryKlass::len(const PyObjPtr& obj) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::len(): obj is not a dict");
  }
  auto dict = obj->as<PyDictionary>();
  return CreatePyInteger(dict->Size());
}

PyObjPtr DictionaryKlass::contains(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(DictionaryKlass::Self())) {
    throw std::runtime_error("PyDictionary::contains(): obj is not a dict");
  }
  auto dict = obj->as<PyDictionary>();
  return PyBoolean::Create(dict->Contains(key));
}

// bool KeyCompare(const PyObjPtr& lhs, const PyObjPtr& rhs) {
//   auto leftHash =
//     lhs->Hashed() ? lhs->HashValue() : lhs->hash()->as<PyInteger>()->ToU64();
//   auto rightHash =
//     rhs->Hashed() ? rhs->HashValue() : rhs->hash()->as<PyInteger>()->ToU64();
//   return leftHash < rightHash;
// }

auto DictClear(const PyObjPtr& obj) -> PyObjPtr {
  auto argList = obj->as<PyList>();
  auto dict = argList->GetItem(0)->as<PyDictionary>();
  dict->Clear();
  return CreatePyNone();
}

auto DictItems(const PyObjPtr& obj) -> PyObjPtr {
  auto argList = obj->as<PyList>();
  auto dict = argList->GetItem(0)->as<PyDictionary>();
  auto items = CreatePyList();
  for (const auto& item : dict->Dictionary()) {
    items->Append(CreatePyList({item.first, item.second}));
  }
  return items;
}

auto DictGet(const PyObjPtr& obj) -> PyObjPtr {
  auto argList = obj->as<PyList>();
  auto dict = argList->GetItem(0)->as<PyDictionary>();
  return dict->Get(argList->GetItem(1));
}
}  // namespace kaubo::Object