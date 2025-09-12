#pragma once

#include "Collections/String/String.h"
#include "Collections/String/StringHelper.h"
#include "Object/Core/IObjectCreator.h"
#include "Object/Core/PyObject.h"
#include "Object/Object.h"

#include <unordered_map>
#include <utility>
namespace kaubo::Object {

class StringKlass : public KlassBase<StringKlass> {
 public:
  explicit StringKlass() = default;

  void Initialize() override;

  PyObjPtr init(const PyObjPtr& klass, const PyObjPtr& args) override;
  PyObjPtr add(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr eq(const PyObjPtr& lhs, const PyObjPtr& rhs) override;
  PyObjPtr len(const PyObjPtr& obj) override;
  PyObjPtr str(const PyObjPtr& obj) override;
  PyObjPtr repr(const PyObjPtr& obj) override;
  PyObjPtr iter(const PyObjPtr& obj) override;
  PyObjPtr hash(const PyObjPtr& obj) override;
  PyObjPtr boolean(const PyObjPtr& obj) override;
  PyObjPtr _serialize_(const PyObjPtr& obj) override;
};

class PyString : public PyObject, public IObjectCreator<PyString> {
  friend class StringKlass;

 private:
  Collections::String m_value;

  static std::unordered_map<size_t, PyStrPtr>& GetStringPool() {
    static std::unordered_map<size_t, PyStrPtr> pool;
    return pool;
  }

 public:
  explicit PyString(Collections::String value)
    : PyObject(StringKlass::Self()), m_value(std::move(value)) {}

  explicit PyString(const char* value)
    : PyObject(StringKlass::Self()),
      m_value(Collections::CreateStringWithCString(value)) {}

  explicit PyString(const std::string& value)
    : PyObject(StringKlass::Self()),
      m_value(Collections::CreateStringWithCString(value.c_str())) {}
  Index Length() { return m_value.GetCodePointCount(); }

  PyStrPtr GetItem(Index index);

  PyStrPtr Join(const PyObjPtr& iterable);

  //  PyListPtr Split(const PyStrPtr& delimiter);

  PyStrPtr Add(const PyStrPtr& other);

  void Print() const;

  std::string ToCppString() const;

  bool Equal(const PyStrPtr& other);

  PyStrPtr Upper();

  Collections::String Value() const { return m_value.Copy(); }
  static PyStrPtr Intern(const Collections::String& value);
  size_t Hash() { return m_value.HashValue(); }

  template <typename... Args>
  static PyStrPtr Create(Args&&... args) {
    auto tempStr =
      IObjectCreator<PyString>::Create(std::forward<Args>(args)...);
    return Intern(tempStr->m_value);
  }
};

using PyStrPtr = std::shared_ptr<PyString>;

PyObjPtr StringUpper(const PyObjPtr& args);

PyObjPtr StringJoin(const PyObjPtr& args);

// PyObjPtr StringSplit(const PyObjPtr& args);

PyObjPtr StringConcat(const PyObjPtr& args);

}  // namespace kaubo::Object
