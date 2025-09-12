#include "Object/Container/PyList.h"
#include <cassert>
#include "ByteCode/ByteCode.h"
#include "Collections/Integer/Integer.h"
#include "Collections/String/BytesHelper.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Core/PyType.h"
#include "Object/Function/FunctionForward.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Iterator/Iterator.h"
#include "Object/Iterator/IteratorHelper.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/PySlice.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyList::PyList(ExpandAndFill reserve) : PyObject(ListKlass::Self()) {
  if (reserve.capacity == 0) {
    return;
  }
  m_list.Expand(reserve.capacity);
  m_list.Fill(PyNone::Create());
}

PyList::PyList(ExpandOnly reserve) : PyObject(ListKlass::Self()) {
  if (reserve.capacity == 0) {
    return;
  }
  m_list.Expand(reserve.capacity);
}

void ListKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  auto* instance = Self();
  InitKlass(PyString::Create("list")->as<PyString>(), instance);
  instance->AddAttribute(
    PyString::Create("append")->as<PyString>(),
    PyNativeFunction::Create(ListAppend)
  );
  instance->AddAttribute(
    PyString::Create("extend")->as<PyString>(),
    PyNativeFunction::Create(ListExtend)
  );
  instance->AddAttribute(
    PyString::Create("index")->as<PyString>(),
    PyNativeFunction::Create(ListIndex)
  );
  instance->AddAttribute(
    PyString::Create("pop")->as<PyString>(), PyNativeFunction::Create(ListPop)
  );
  instance->AddAttribute(
    PyString::Create("remove")->as<PyString>(),
    PyNativeFunction::Create(ListRemove)
  );
  instance->AddAttribute(
    PyString::Create("reverse")->as<PyString>(),
    PyNativeFunction::Create(ListReverse)
  );
  instance->AddAttribute(
    PyString::Create("clear")->as<PyString>(),
    PyNativeFunction::Create(ListClear)
  );
  instance->AddAttribute(
    PyString::Create("copy")->as<PyString>(), PyNativeFunction::Create(ListCopy)
  );
  instance->AddAttribute(
    PyString::Create("count")->as<PyString>(),
    PyNativeFunction::Create(ListCount)
  );
  instance->AddAttribute(
    PyString::Create("insert")->as<PyString>(),
    PyNativeFunction::Create(ListInsert)
  );
  // 注册重载函数
  instance->AddAttribute(
    PyString::Create("__getitem__")->as<PyString>(),
    PyNativeFunction::Create(
      CreateForwardFunction<ListKlass>(&ListKlass::getitem)
    )
  );
  instance->AddAttribute(
    PyString::Create("__setitem__")->as<PyString>(),
    PyNativeFunction::Create(
      CreateForwardFunction<ListKlass>(&ListKlass::setitem)
    )
  );
  instance->AddAttribute(
    PyString::Create("__len__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::len))
  );
  instance->AddAttribute(
    PyString::Create("__contains__")->as<PyString>(),
    PyNativeFunction::Create(
      CreateForwardFunction<ListKlass>(&ListKlass ::contains)
    )
  );
  instance->AddAttribute(
    PyString::Create("__iter__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::iter))
  );
  instance->AddAttribute(
    PyString::Create("__add__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::add))
  );
  instance->AddAttribute(
    PyString::Create("__mul__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::mul))
  );
  instance->AddAttribute(
    PyString::Create("__str__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::str))
  );
  instance->AddAttribute(
    PyString::Create("__repr__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::repr))
  );
  instance->AddAttribute(
    PyString::Create("__eq__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::eq))
  );
  instance->AddAttribute(
    PyString::Create("__init__")->as<PyString>(),
    PyNativeFunction::Create(CreateForwardFunction<ListKlass>(&ListKlass::init))
  );
  instance->AddAttribute(
    PyString::Create("__serialize__")->as<PyString>(),
    PyNativeFunction::Create(
      CreateForwardFunction<ListKlass>(&ListKlass::_serialize_)
    )
  );
  this->SetInitialized();
}

PyObjPtr ListKlass::init(const PyObjPtr& type, const PyObjPtr& args) {
  if (type->as<PyType>()->Owner() != Self()) {
    throw std::runtime_error("List does not support init operation");
  }
  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("List allocation argument must be a list");
  }
  auto argList = args->as<PyList>();
  if (argList->Length() == 0) {
    return PyList::Create();
  }
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto value = argList->GetItem(0);
  if (!value->is(ListKlass::Self())) {
    throw std::runtime_error("List allocation argument must be a list");
  }
  return value;
}

PyObjPtr ListKlass::add(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(ListKlass::Self()) || !rhs->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support add operation");
  }
  return lhs->as<PyList>()->Add(rhs->as<PyList>());
}

PyObjPtr ListKlass::mul(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(ListKlass::Self()) || !rhs->is(IntegerKlass::Self())) {
    throw std::runtime_error("List does not support mul operation");
  }
  auto list = lhs->as<PyList>();
  auto times = rhs->as<PyInteger>()->ToU64();
  if (list->Length() == 1) {
    auto value = list->GetItem(0);
    Collections::List<PyObjPtr> result(times, value);
    return PyList::Create(result);
  }
  Collections::List<PyObjPtr> result(list->Length() * times);
  for (Index i = 0; i < times; i++) {
    for (Index j = 0; j < list->Length(); j++) {
      result.Push(list->GetItem(j));
    }
  }
  return PyList::Create(result);
}

PyObjPtr ListKlass::str(const PyObjPtr& obj) {
  auto list = obj->as<PyList>();
  auto strList = Map(list, [](const PyObjPtr& value) { return value->repr(); });
  return StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create("[")->as<PyString>(),
       PyString::Create(", ")->as<PyString>()->Join(strList),
       PyString::Create("]")->as<PyString>()}
    )
  );
}

PyObjPtr ListKlass::repr(const PyObjPtr& obj) {
  auto list = obj->as<PyList>();
  auto reprList =
    Map(list, [](const PyObjPtr& value) { return value->repr(); });
  return StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create("["),
       PyString::Create(", ")->as<PyString>()->Join(reprList),
       PyString::Create("]")}
    )
  );
}

PyObjPtr ListKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(ListKlass::Self()) || !rhs->is(ListKlass::Self())) {
    return PyBoolean::Create(false);
  }
  auto left = lhs->as<PyList>();
  auto right = rhs->as<PyList>();
  if (left->Length() != right->Length()) {
    return PyBoolean::Create(false);
  }
  for (Index i = 0; i < left->Length(); i++) {
    if (left->GetItem(i) != right->GetItem(i)) {
      return PyBoolean::Create(false);
    }
  }
  return PyBoolean::Create(true);
}

PyObjPtr ListKlass::getitem(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(ListKlass::Self())) {
    auto errorMessage = StringConcat(
      PyList::Create<Object::PyObjPtr>(
        {PyString::Create("AttributeError: '"), obj->Klass()->Name(),
         PyString::Create("' object has no attribute '__getitem__'")}
      )
    );
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  auto list = obj->as<PyList>();
  if (key->is(IntegerKlass::Self())) {
    auto index = key->as<PyInteger>();
    if (index->GetSign() == Collections::Integer::IntSign::Positive) {
      return list->GetItem(index->ToU64());
    }
    if (index->GetSign() == Collections::Integer::IntSign::Negative) {
      //      return list->GetItem(list->Length() + index->ToI64());
      // warning: implicit conversion changes signedness: 'int64_t' (aka 'long
      // long') to 'Index' (aka 'unsigned long long')
      // [clang-diagnostic-sign-conversion]
      return list->GetItem(
        static_cast<Index>(
          static_cast<int64_t>(list->Length()) + index->ToI64()
        )
      );
    }
  }
  if (key->is(SliceKlass::Self())) {
    auto slice = key->as<PySlice>();
    return list->GetSlice(slice);
  }
  if (key->is(ListKlass::Self())) {
    auto indexList = key->as<PyList>();
    auto result = obj;
    for (Index i = 0; i < indexList->Length(); i++) {
      result = result->getitem(indexList->GetItem(i));
    }
    return result;
  }
  auto errorMessage = StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create(
         "TypeError: list indices must be integers or slices, not '"
       ),
       key->Klass()->Name(), PyString::Create("'")}
    )
  );
  throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
}

PyObjPtr ListKlass::setitem(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  if (!obj->is(ListKlass::Self())) {
    auto errorMessage = StringConcat(
      PyList::Create<Object::PyObjPtr>(
        {PyString::Create("AttributeError: '"), obj->Klass()->Name(),
         PyString::Create("' object has no attribute '__setitem__'")}
      )
    );
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  auto list = obj->as<PyList>();
  // list[key] = value
  if (key->is(IntegerKlass::Self())) {
    auto index = key->as<PyInteger>()->ToU64();
    list->SetItem(index, value);
    return PyNone::Create();
  }
  // list[start:stop] = iterable
  if (key->is(SliceKlass::Self())) {
    auto slice = key->as<PySlice>();
    if (!slice->GetStep()->is(NoneKlass::Self())) {
      throw std::runtime_error(
        "List does not support step in slice assignment"
      );
    }
    slice->BindLength(list->Length());
    auto stop = slice->GetStop()->as<PyInteger>()->ToU64();
    auto start = slice->GetStart()->as<PyInteger>()->ToU64();
    auto valueList = PyList::Create(value);
    list->InsertAndReplace(start, stop, valueList);
    return PyNone::Create();
  }
  if (key->is(ListKlass::Self())) {
    auto indexList = key->as<PyList>();
    auto result = obj;
    for (Index i = 0; i < indexList->Length() - 1; i++) {
      result = result->getitem(indexList->GetItem(i));
    }
    result->setitem(indexList->GetItem(indexList->Length() - 1), value);
    return PyNone::Create();
  }
  auto errorMessage = StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create(
         "TypeError: list indices must be integers or slices, not '"
       ),
       key->Klass()->Name(), PyString::Create("'")}
    )
  );
  throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
}

PyObjPtr ListKlass::len(const PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    auto errorMessage = StringConcat(
      PyList::Create<Object::PyObjPtr>(
        {PyString::Create("TypeError: unsupported operand type(s) for"),
         PyString::Create(" len(): '"), obj->Klass()->Name(),
         PyString::Create("'")}
      )
    );
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  return PyInteger::Create(obj->as<PyList>()->Length());
}

PyObjPtr ListKlass::contains(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support contains operation");
  }
  return PyBoolean::Create(obj->as<PyList>()->Contains(key));
}

PyObjPtr ListKlass::boolean(const PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support boolean operation");
  }
  return PyBoolean::Create(obj->as<PyList>()->Length() > 0);
}

PyObjPtr ListKlass::_serialize_(const PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support serialize operation");
  }
  auto list = obj->as<PyList>();
  Collections::StringBuilder bytes(Collections::Serialize(Literal::LIST));
  bytes.Append(Collections::Serialize(list->Length()));
  for (Index i = 0; i < list->Length(); i++) {
    auto value = list->GetItem(i);
    bytes.Append(value->_serialize_()->as<PyBytes>()->Value());
  }
  return PyBytes::Create(bytes.ToString());
}

PyObjPtr ListKlass::iter(const PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support iter operation");
  }
  return CreateListIterator(obj);
}

PyObjPtr ListKlass::reversed(const kaubo::Object::PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support reversed operation");
  }
  return CreateListReverseIterator(obj);
}

PyObjPtr ListAppend(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  list->Append(obj);
  return PyNone::Create();
}

PyObjPtr ListExtend(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  ForEach(obj, [&list](const PyObjPtr& value) { list->Append(value); });
  return PyNone::Create();
}

PyObjPtr ListIndex(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto funcName = PyString::Create("list_index")->as<PyString>();
  CheckNativeFunctionArgumentsAtIndexWithType(
    funcName, args, 0, ListKlass::Self()
  );
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  return PyInteger::Create(list->IndexOf(obj));
}

PyObjPtr ListPop(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto argList = args->as<PyList>();
  auto list = argList->GetItem(0)->as<PyList>();
  auto index = list->Length() - 1;
  if (args->as<PyList>()->Length() == 2) {
    auto argIndex = args->as<PyList>()->GetItem(1)->as<PyInteger>()->ToI64();
    if (argIndex < 0) {
      int64_t temp = static_cast<int64_t>(list->Length()) + argIndex;
      assert(temp >= 0 && "Negative index out of range");
      index = static_cast<Index>(temp);
    }
  }
  return list->Pop(index);
}

PyObjPtr ListClear(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto list = args->as<PyList>()->GetItem(0)->as<PyList>();
  list->Clear();
  return PyNone::Create();
}

PyObjPtr ListRemove(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  if (!list->Contains(obj)) {
    throw std::runtime_error("List does not contain the object");
  }
  list->RemoveAt(list->IndexOf(obj));
  return PyNone::Create();
}

PyObjPtr ListCount(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  Index count = 0;
  for (Index i = 0; i < list->Length(); i++) {
    if (list->GetItem(i) == obj) {
      count++;
    }
  }
  return PyInteger::Create(count);
}

PyObjPtr ListReverse(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto list = args->as<PyList>()->GetItem(0)->as<PyList>();
  list->Reverse();
  return PyNone::Create();
}

PyObjPtr ListCopy(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto list = args->as<PyList>()->GetItem(0)->as<PyList>();
  return list->Copy();
}

PyObjPtr ListInsert(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 3);
  auto argList = args->as<PyList>();
  auto index = argList->GetItem(1)->as<PyInteger>()->ToI64();
  auto list = argList->GetItem(0)->as<PyList>();
  Index actualIndex = 0;
  if (index < 0) {
    int64_t temp = static_cast<int64_t>(list->Length()) + index;
    assert(temp >= 0 && "Negative index out of range");
    actualIndex = static_cast<Index>(temp);
  } else {
    actualIndex = static_cast<Index>(index);
  }
  auto obj = argList->GetItem(2);
  list->Insert(actualIndex, obj);
  return PyNone::Create();
}

PyObjPtr PyList::GetSlice(const PySlicePtr& slice) const {
  if (slice->GetStep()->is(NoneKlass::Self())) {
    slice->BindLength(Length());
    auto start = slice->GetStart()->as<PyInteger>()->ToU64();
    auto stop = slice->GetStop()->as<PyInteger>()->ToU64();
    return PyList::Create(m_list.Slice(start, stop))->as<PyList>();
  }
  slice->BindLength(Length());
  int64_t start = slice->GetStart()->as<PyInteger>()->ToI64();
  int64_t stop = slice->GetStop()->as<PyInteger>()->ToI64();
  int64_t step = slice->GetStep()->as<PyInteger>()->ToI64();
  auto subList = PyList::Create()->as<PyList>();
  if (step > 0) {
    for (int64_t i = start; i < stop; i += step) {
      if (i >= static_cast<int64_t>(Length())) {
        break;
      }
      subList->Append(GetItem(static_cast<Index>(i)));
    }
    return subList;
  }
  for (int64_t i = start; i > stop; i += step) {
    if (i >= static_cast<int64_t>(Length())) {
      break;
    }
    subList->Append(GetItem(static_cast<Index>(i)));
  }
  return subList;
}

PyList::PyList(const PyObjPtr& iterator) : PyObject(ListKlass::Self()) {
  auto iter = iterator->iter();
  auto value = iter->next();
  Collections::List<PyObjPtr> list;
  while (!value->is(IterDoneKlass::Self())) {
    list.Push(value);
    value = iter->next();
  }
  this->m_list = list;
}

}  // namespace kaubo::Object