#include "Object/String/PyString.h"
#include "ByteCode/ByteCode.h"
#include "Collections/String/BytesHelper.h"
#include "Collections/String/StringHelper.h"
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyType.h"
#include "Object/Function/FunctionForward.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Iterator/Iterator.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "PyBytes.h"

#include "Tools/Terminal/Terminal.h"

namespace kaubo::Object {

void StringKlass::Initialize() {
  InitKlass(PyString::Create("str"), Self());
  Self()->AddAttribute(
    PyString::Create("join"), CreatePyNativeFunction(StringJoin)
  );
  //  Self()->AddAttribute(
  //    PyString::Create("split")->as<PyString>(),
  //    CreatePyNativeFunction(StringSplit)
  //  );
  Self()->AddAttribute(
    PyString::Create("upper"), CreatePyNativeFunction(StringUpper)
  );
  Self()->AddAttribute(
    PyString::Create("__add__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::add)
    )
  );
  Self()->AddAttribute(
    PyString::Create("__eq__"),
    CreatePyNativeFunction(CreateForwardFunction<StringKlass>(&StringKlass::eq))
  );
  Self()->AddAttribute(
    PyString::Create("__len__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::len)
    )
  );
  Self()->AddAttribute(
    PyString::Create("__str__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::str)
    )
  );
  Self()->AddAttribute(
    PyString::Create("__repr__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::repr)
    )
  );
  Self()->AddAttribute(
    PyString::Create("__iter__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::iter)
    )
  );
  Self()->AddAttribute(
    PyString::Create("__serialize__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::_serialize_)
    )
  );
  Self()->AddAttribute(
    PyString::Create("__init__"),
    CreatePyNativeFunction(
      CreateForwardFunction<StringKlass>(&StringKlass::init)
    )
  );
}

PyObjPtr StringKlass::init(const PyObjPtr& klass, const PyObjPtr& args) {
  if (klass->as<PyType>()->Owner() != Self()) {
    throw std::runtime_error("init(): klass is not a string");
  }
  auto argList = args->as<PyList>();
  if (argList->Length() == 0) {
    return PyString::Create("");
  }
  if (argList->Length() != 1) {
    throw std::runtime_error("init(): args must be a list with one element");
  }
  return argList->GetItem(0)->str();
}

PyObjPtr StringKlass::add(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(StringKlass::Self()) || !rhs->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::add(): lhs or rhs is not a string");
  }
  auto left = lhs->as<PyString>();
  auto right = rhs->as<PyString>();
  return left->Add(right);
}

PyObjPtr StringKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(StringKlass::Self()) || !rhs->is(StringKlass::Self())) {
    return PyBoolean::Create(false);
  }
  auto left = lhs->as<PyString>();
  auto right = rhs->as<PyString>();
  return PyBoolean::Create(left->Equal(right));
}

PyObjPtr StringKlass::len(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::len(): obj is not a string");
  }
  auto string = obj->as<PyString>();
  return CreatePyInteger(string->Length());
}

PyObjPtr StringKlass::str(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::str(): obj is not a string");
  }
  return obj;
}

PyObjPtr StringKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::repr(): obj is not a string");
  }
  auto string = obj->as<PyString>();
  return StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create("'"), string, PyString::Create("'")}
    )
  );
}

PyObjPtr StringKlass::iter(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::iter(): obj is not a string");
  }
  return CreateStringIterator(obj);
}

PyObjPtr StringKlass::hash(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::hash(): obj is not a string");
  }
  if (obj->Hashed()) {
    return CreatePyInteger(obj->HashValue());
  }
  auto string = obj->as<PyString>();
  size_t hash = string->Hash();
  string->SetHashValue(hash);
  return CreatePyInteger(hash);
}

PyObjPtr StringKlass::boolean(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::boolean(): obj is not a string");
  }
  auto string = obj->as<PyString>();
  return PyBoolean::Create(string->Length() > 0);
}

PyObjPtr StringKlass::_serialize_(const PyObjPtr& obj) {
  if (!obj->is(StringKlass::Self())) {
    throw std::runtime_error("StringKlass::_serialize_(): obj is not a string");
  }
  Collections::StringBuilder bytes(Collections::Serialize(Literal::STRING));
  bytes.Append(Collections::Serialize(obj->as<PyString>()->m_value));
  return PyBytes::Create(bytes.ToString());
}

PyStrPtr PyString::GetItem(Index index) {
  if (index >= m_value.GetCodePointCount()) {
    throw std::runtime_error("PyString::GetItem(): index out of range");
  }
  return PyString::Create(Collections::String(m_value.Slice(index, index + 1)));
}

PyStrPtr PyString::Join(const PyObjPtr& iterable) {
  Collections::StringBuilder stringBuilder;
  for (Index i = 0; i < iterable->as<PyList>()->Length(); i++) {
    auto item = iterable->as<PyList>()->GetItem(i);
    if (i == 0) {
      stringBuilder.Append(item->as<PyString>()->m_value);
    } else {
      stringBuilder.Append(m_value);
      stringBuilder.Append(item->as<PyString>()->m_value);
    }
  }
  return PyString::Create(stringBuilder.ToString())->as<PyString>();
}

// PyListPtr PyString::Split(const PyStrPtr& delimiter) {
//   auto parts = value.Split(delimiter->value);
//   auto result = PyList::Create(parts.Size())->as<PyList>();
//   for (Index i = 0; i < parts.Size(); i++) {
//     result->SetItem(i, PyString::Create(parts[i]));
//   }
//   return result;
// }

PyStrPtr PyString::Add(const PyStrPtr& other) {
  return PyString::Create(m_value.Add(other->m_value));
}

void PyString::Print() const {
  ConsoleTerminal::get_instance().info(ToCppString());
}

std::string PyString::ToCppString() const {
  return m_value.ToCppString();
}

bool PyString::Equal(const PyStrPtr& other) {
  return m_value.Equal(other->m_value);
}

PyStrPtr PyString::Upper() {
  return PyString::Create(m_value.Upper());
}

PyObjPtr StringConcat(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto argList = args->as<PyList>();
  auto funcName = PyString::Create("StringConcat")->as<PyString>();
  auto result = PyString::Create("")->as<PyString>();
  for (Index i = 0; i < argList->Length(); i++) {
    auto value = argList->GetItem(i);
    CheckNativeFunctionArgumentWithType(
      funcName, value, i, StringKlass::Self()
    );
    result = result->Add(value->as<PyString>());
  }
  return result;
}

PyObjPtr StringUpper(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto funcName = PyString::Create("StringUpper")->as<PyString>();
  CheckNativeFunctionArgumentsAtIndexWithType(
    funcName, args, 0, StringKlass::Self()
  );
  auto argList = args->as<PyList>();
  auto value = argList->GetItem(0)->as<PyString>();
  return value->Upper();
}

PyObjPtr StringJoin(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<PyList>();
  auto delimiter = argList->GetItem(0)->as<PyString>();
  auto list = argList->GetItem(1)->as<PyList>();
  auto strList = Map(list, [](const PyObjPtr& item) { return item->str(); });
  return delimiter->Join(strList);
}

// PyObjPtr StringSplit(const PyObjPtr& args) {
//   CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
//   auto argList = args->as<PyList>();
//   auto delimiter = argList->GetItem(0)->as<PyString>();
//   auto value = argList->GetItem(1)->as<PyString>();
//   return value->Split(delimiter);
// }

PyStrPtr PyString::Intern(const Collections::String& value) {
  auto& poolInstance = GetStringPool();
  auto hash = value.HashValue();
  auto iter = poolInstance.find(hash);
  if (iter != poolInstance.end()) {
    return iter->second;
  }
  auto result = std::make_shared<PyString>(value);
  poolInstance[hash] = result;
  return result;
}

}  // namespace kaubo::Object