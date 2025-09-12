#pragma once

#include "Collections/List.h"
#include "Object/Core/IObjectCreator.h"
#include "Object/Core/PyObject.h"
#include "Object/Object.h"
#include "Object/PySlice.h"

namespace kaubo::Object {
class ListKlass : public KlassBase<ListKlass> {
 public:
  explicit ListKlass() = default;

  PyObjPtr add(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr mul(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr str(const PyObjPtr& obj) override;
  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr getitem(const PyObjPtr& obj, const PyObjPtr& key) override;
  PyObjPtr setitem(
    const PyObjPtr& obj,
    const PyObjPtr& key,
    const PyObjPtr& value
  ) override;
  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr len(const PyObjPtr& obj) override;
  PyObjPtr contains(const PyObjPtr& obj, const PyObjPtr& key) override;
  PyObjPtr init(const PyObjPtr& type, const PyObjPtr& args) override;
  PyObjPtr iter(const PyObjPtr& obj) override;
  PyObjPtr boolean(const PyObjPtr& obj) override;
  PyObjPtr reversed(const PyObjPtr& obj) override;
  PyObjPtr _serialize_(const PyObjPtr& obj) override;
  void Initialize() override;
};

class PyList;
using PyListPtr = std::shared_ptr<PyList>;
class PyList : public PyObject, public IObjectCreator<PyList> {
 private:
  Collections::List<PyObjPtr> m_list;

 public:
  struct ExpandOnly {
    Index capacity;
  };

  struct ExpandAndFill {
    Index capacity;
  };
  explicit PyList(Collections::List<PyObjPtr> value)
    : PyObject(ListKlass::Self()), m_list(std::move(value)) {}
  explicit PyList(ExpandOnly reserve);
  explicit PyList(ExpandAndFill reserve);
  PyList(std::initializer_list<PyObjPtr> list)
    : PyObject(ListKlass::Self()), m_list(list) {}
  explicit PyList() : PyObject(ListKlass::Self()) {}

  explicit PyList(const PyObjPtr& iterator);

  void Shuffle() { m_list.Shuffle(); }
  void Append(const PyObjPtr& obj) { m_list.Push(obj); }
  PyObjPtr Add(const PyObjPtr& obj) {
    return PyList::Create(m_list.Add(obj->as<PyList>()->m_list));
  }
  Index Length() const { return m_list.Size(); }
  bool Contains(const PyObjPtr& obj) const { return m_list.Contains(obj); }
  Index IndexOf(const PyObjPtr& obj) const { return m_list.IndexOf(obj); }
  PyObjPtr GetItem(Index index) const { return m_list[index]; }
  PyObjPtr GetSlice(const PySlicePtr& slice) const;
  void SetItem(Index index, const PyObjPtr& obj) { m_list.Set(index, obj); }
  PyObjPtr Prepend(const PyObjPtr& obj) {
    return PyList::Create({obj})->Add(shared_from_this());
  }
  void RemoveAt(Index index) { m_list.RemoveAt(index); }
  void InsertAndReplace(Index start, Index end, const PyListPtr& list) {
    m_list.InsertAndReplace(start, end, list->m_list);
  }
  PyObjPtr Pop(Index index) {
    auto obj = m_list[index];
    m_list.RemoveAt(index);
    return obj;
  }
  void Clear() { m_list.Clear(); }
  void Reverse() { m_list.Reverse(); }
  PyListPtr Copy() { return PyList::Create(m_list.Copy())->as<PyList>(); }
  void Insert(Index index, const PyObjPtr& obj) { m_list.Insert(index, obj); };
};

PyObjPtr ListIndex(const PyObjPtr& args);

PyObjPtr ListAppend(const PyObjPtr& args);

PyObjPtr ListExtend(const PyObjPtr& args);

PyObjPtr ListCount(const PyObjPtr& args);

PyObjPtr ListPop(const PyObjPtr& args);

PyObjPtr ListInsert(const PyObjPtr& args);

PyObjPtr ListRemove(const PyObjPtr& args);

PyObjPtr ListReverse(const PyObjPtr& args);

PyObjPtr ListClear(const PyObjPtr& args);

PyObjPtr ListCopy(const PyObjPtr& args);

// PyListPtr PyList::CreateFromIterable(const PyObjPtr& iterator);
}  // namespace kaubo::Object
