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

PyListPtr CreatePyList(Index capacity) {
  if (capacity == 0) {
    return std::make_shared<PyList>(Collections::List<PyObjPtr>());
  }
  Collections::List<PyObjPtr> list(capacity);
  list.Fill(CreatePyNone());
  return std::make_shared<PyList>(list);
}

PyListPtr CreatePyListWithNullPtr(Index capacity) {
  if (capacity == 0) {
    return std::make_shared<PyList>(Collections::List<PyObjPtr>());
  }
  Collections::List<PyObjPtr> list(capacity);
  return std::make_shared<PyList>(list);
}

PyListPtr CreatePyList() {
  return std::make_shared<PyList>(Collections::List<PyObjPtr>());
}

PyListPtr CreatePyList(const Collections::List<PyObjPtr>& list) {
  return std::make_shared<PyList>(list);
}

PyListPtr CreatePyList(std::initializer_list<PyObjPtr> list) {
  return std::make_shared<PyList>(Collections::List<PyObjPtr>(list));
}

void ListKlass::Initialize() {
  if (this->IsInitialized()) {
    return;
  }
  auto* instance = Self();
  InitKlass(CreatePyString("list")->as<PyString>(), instance);
  instance->AddAttribute(
    CreatePyString("append")->as<PyString>(), CreatePyNativeFunction(ListAppend)
  );
  instance->AddAttribute(
    CreatePyString("extend")->as<PyString>(), CreatePyNativeFunction(ListExtend)
  );
  instance->AddAttribute(
    CreatePyString("index")->as<PyString>(), CreatePyNativeFunction(ListIndex)
  );
  instance->AddAttribute(
    CreatePyString("pop")->as<PyString>(), CreatePyNativeFunction(ListPop)
  );
  instance->AddAttribute(
    CreatePyString("remove")->as<PyString>(), CreatePyNativeFunction(ListRemove)
  );
  instance->AddAttribute(
    CreatePyString("reverse")->as<PyString>(),
    CreatePyNativeFunction(ListReverse)
  );
  instance->AddAttribute(
    CreatePyString("clear")->as<PyString>(), CreatePyNativeFunction(ListClear)
  );
  instance->AddAttribute(
    CreatePyString("copy")->as<PyString>(), CreatePyNativeFunction(ListCopy)
  );
  instance->AddAttribute(
    CreatePyString("count")->as<PyString>(), CreatePyNativeFunction(ListCount)
  );
  instance->AddAttribute(
    CreatePyString("insert")->as<PyString>(), CreatePyNativeFunction(ListInsert)
  );
  // 注册重载函数
  instance->AddAttribute(
    CreatePyString("__getitem__")->as<PyString>(),
    CreatePyNativeFunction(
      CreateForwardFunction<ListKlass>(&ListKlass::getitem)
    )
  );
  instance->AddAttribute(
    CreatePyString("__setitem__")->as<PyString>(),
    CreatePyNativeFunction(
      CreateForwardFunction<ListKlass>(&ListKlass::setitem)
    )
  );
  instance->AddAttribute(
    CreatePyString("__len__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::len))
  );
  instance->AddAttribute(
    CreatePyString("__contains__")->as<PyString>(),
    CreatePyNativeFunction(
      CreateForwardFunction<ListKlass>(&ListKlass ::contains)
    )
  );
  instance->AddAttribute(
    CreatePyString("__iter__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::iter))
  );
  instance->AddAttribute(
    CreatePyString("__add__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::add))
  );
  instance->AddAttribute(
    CreatePyString("__mul__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::mul))
  );
  instance->AddAttribute(
    CreatePyString("__str__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::str))
  );
  instance->AddAttribute(
    CreatePyString("__repr__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::repr))
  );
  instance->AddAttribute(
    CreatePyString("__eq__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::eq))
  );
  instance->AddAttribute(
    CreatePyString("__init__")->as<PyString>(),
    CreatePyNativeFunction(CreateForwardFunction<ListKlass>(&ListKlass::init))
  );
  instance->AddAttribute(
    CreatePyString("__serialize__")->as<PyString>(),
    CreatePyNativeFunction(
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
    return CreatePyList();
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
    return CreatePyList(result);
  }
  Collections::List<PyObjPtr> result(list->Length() * times);
  for (Index i = 0; i < times; i++) {
    for (Index j = 0; j < list->Length(); j++) {
      result.Push(list->GetItem(j));
    }
  }
  return CreatePyList(result);
}

PyObjPtr ListKlass::str(const PyObjPtr& obj) {
  auto list = obj->as<PyList>();
  auto strList = Map(list, [](const PyObjPtr& value) { return value->repr(); });
  return StringConcat(CreatePyList(
    {CreatePyString("[")->as<PyString>(),
     CreatePyString(", ")->as<PyString>()->Join(strList),
     CreatePyString("]")->as<PyString>()}
  ));
}

PyObjPtr ListKlass::repr(const PyObjPtr& obj) {
  auto list = obj->as<PyList>();
  auto reprList =
    Map(list, [](const PyObjPtr& value) { return value->repr(); });
  return StringConcat(CreatePyList(
    {CreatePyString("["), CreatePyString(", ")->as<PyString>()->Join(reprList),
     CreatePyString("]")}
  ));
}

PyObjPtr ListKlass::eq(const PyObjPtr& lhs, const PyObjPtr& rhs) {
  if (!lhs->is(ListKlass::Self()) || !rhs->is(ListKlass::Self())) {
    return PyBoolean::create(false);
  }
  auto left = lhs->as<PyList>();
  auto right = rhs->as<PyList>();
  if (left->Length() != right->Length()) {
    return PyBoolean::create(false);
  }
  for (Index i = 0; i < left->Length(); i++) {
    if (left->GetItem(i) != right->GetItem(i)) {
      return PyBoolean::create(false);
    }
  }
  return PyBoolean::create(true);
}

PyObjPtr ListKlass::getitem(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(ListKlass::Self())) {
    auto errorMessage = StringConcat(CreatePyList(
      {CreatePyString("AttributeError: '"), obj->Klass()->Name(),
       CreatePyString("' object has no attribute '__getitem__'")}
    ));
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
  auto errorMessage = StringConcat(CreatePyList(
    {CreatePyString(
       "TypeError: list indices must be integers or slices, not '"
     ),
     key->Klass()->Name(), CreatePyString("'")}
  ));
  throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
}

PyObjPtr ListKlass::setitem(
  const PyObjPtr& obj,
  const PyObjPtr& key,
  const PyObjPtr& value
) {
  if (!obj->is(ListKlass::Self())) {
    auto errorMessage = StringConcat(CreatePyList(
      {CreatePyString("AttributeError: '"), obj->Klass()->Name(),
       CreatePyString("' object has no attribute '__setitem__'")}
    ));
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  auto list = obj->as<PyList>();
  // list[key] = value
  if (key->is(IntegerKlass::Self())) {
    auto index = key->as<PyInteger>()->ToU64();
    list->SetItem(index, value);
    return CreatePyNone();
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
    auto valueList = CreatePyListFromIterable(value);
    list->InsertAndReplace(start, stop, valueList);
    return CreatePyNone();
  }
  if (key->is(ListKlass::Self())) {
    auto indexList = key->as<PyList>();
    auto result = obj;
    for (Index i = 0; i < indexList->Length() - 1; i++) {
      result = result->getitem(indexList->GetItem(i));
    }
    result->setitem(indexList->GetItem(indexList->Length() - 1), value);
    return CreatePyNone();
  }
  auto errorMessage = StringConcat(CreatePyList(
    {CreatePyString(
       "TypeError: list indices must be integers or slices, not '"
     ),
     key->Klass()->Name(), CreatePyString("'")}
  ));
  throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
}

PyObjPtr ListKlass::len(const PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    auto errorMessage = StringConcat(CreatePyList(
      {CreatePyString("TypeError: unsupported operand type(s) for"),
       CreatePyString(" len(): '"), obj->Klass()->Name(), CreatePyString("'")}
    ));
    throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
  }
  return CreatePyInteger(obj->as<PyList>()->Length());
}

PyObjPtr ListKlass::contains(const PyObjPtr& obj, const PyObjPtr& key) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support contains operation");
  }
  return PyBoolean::create(obj->as<PyList>()->Contains(key));
}

PyObjPtr ListKlass::boolean(const PyObjPtr& obj) {
  if (!obj->is(ListKlass::Self())) {
    throw std::runtime_error("List does not support boolean operation");
  }
  return PyBoolean::create(obj->as<PyList>()->Length() > 0);
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
  return CreatePyBytes(bytes.ToString());
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
  return CreatePyNone();
}

PyObjPtr ListExtend(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  ForEach(obj, [&list](const PyObjPtr& value) { list->Append(value); });
  return CreatePyNone();
}

PyObjPtr ListIndex(const PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto funcName = CreatePyString("list_index")->as<PyString>();
  CheckNativeFunctionArgumentsAtIndexWithType(
    funcName, args, 0, ListKlass::Self()
  );
  auto argList = args->as<PyList>();
  auto obj = argList->GetItem(1);
  auto list = argList->GetItem(0)->as<PyList>();
  return CreatePyInteger(list->IndexOf(obj));
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
  return CreatePyNone();
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
  return CreatePyNone();
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
  return CreatePyInteger(count);
}

PyObjPtr ListReverse(const PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto list = args->as<PyList>()->GetItem(0)->as<PyList>();
  list->Reverse();
  return CreatePyNone();
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
  return CreatePyNone();
}

PyObjPtr PyList::GetSlice(const PySlicePtr& slice) const {
  if (slice->GetStep()->is(NoneKlass::Self())) {
    slice->BindLength(Length());
    auto start = slice->GetStart()->as<PyInteger>()->ToU64();
    auto stop = slice->GetStop()->as<PyInteger>()->ToU64();
    return CreatePyList(value.Slice(start, stop))->as<PyList>();
  }
  slice->BindLength(Length());
  int64_t start = slice->GetStart()->as<PyInteger>()->ToI64();
  int64_t stop = slice->GetStop()->as<PyInteger>()->ToI64();
  int64_t step = slice->GetStep()->as<PyInteger>()->ToI64();
  auto subList = CreatePyList()->as<PyList>();
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

PyListPtr CreatePyListFromIterable(const PyObjPtr& iterator) {
  auto iter = iterator->iter();
  auto value = iter->next();
  Collections::List<PyObjPtr> list;
  while (!value->is(IterDoneKlass::Self())) {
    list.Push(value);
    value = iter->next();
  }
  return CreatePyList(list)->as<PyList>();
}

}  // namespace kaubo::Object