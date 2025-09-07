#pragma once

#include "Common.h"
#include "Object/Core/Klass.h"
#include "Object/Core/PyObject.h"

#include <unordered_map>

namespace kaubo::Object {

class DictionaryKlass : public KlassBase<DictionaryKlass> {
 public:
  explicit DictionaryKlass() = default;

  PyObjPtr init(const PyObjPtr& klass, const PyObjPtr& args) override;

  PyObjPtr setitem(
    const PyObjPtr& obj,
    const PyObjPtr& key,
    const PyObjPtr& value
  ) override;

  PyObjPtr getitem(const PyObjPtr& obj, const PyObjPtr& key) override;
  PyObjPtr delitem(const PyObjPtr& obj, const PyObjPtr& key) override;
  PyObjPtr contains(const PyObjPtr& obj, const PyObjPtr& key) override;
  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override;
  PyObjPtr iter(const PyObjPtr& obj) override;
  PyObjPtr len(const PyObjPtr& obj) override;
  void Initialize() override;
};

// bool KeyCompare(const PyObjPtr& lhs, const PyObjPtr& rhs);

class PyDictionary : public PyObject {
 private:
  std::unordered_map<PyObjPtr, PyObjPtr> dict;

 public:
  explicit PyDictionary() : PyObject(DictionaryKlass::Self()) {}

  void Put(const PyObjPtr& key, const PyObjPtr& value);

  PyObjPtr Get(const PyObjPtr& key);

  void Remove(const PyObjPtr& key);

  bool Contains(const PyObjPtr& key);

  Index Size() const;

  PyObjPtr GetItem(Index index) const;

  PyObjPtr TryGet(const PyObjPtr& key) const;

  PyDictPtr Add(const PyDictPtr& other);
  void Clear() { dict.clear(); }
  auto Dictionary() -> decltype(dict) { return dict; }
};

PyObjPtr CreatePyDict();
using PyDictPtr = std::shared_ptr<PyDictionary>;

auto DictClear(const PyObjPtr& obj) -> PyObjPtr;
auto DictItems(const PyObjPtr& obj) -> PyObjPtr;
auto DictGet(const PyObjPtr& obj) -> PyObjPtr;
}  // namespace kaubo::Object
