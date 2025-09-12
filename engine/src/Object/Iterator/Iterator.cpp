#include "Object/Iterator/Iterator.h"
#include "Object/Container/PyList.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"

namespace kaubo::Object {

PyObjPtr ListIteratorKlass::next(const PyObjPtr& obj) {
  auto iterator = obj->as<ListIterator>();
  auto list = iterator->List();
  if (iterator->CurrentIndex() >= list->Length()) {
    return CreateIterDone();
  }
  auto value = list->GetItem(iterator->CurrentIndex());
  iterator->Next();
  return value;
}

PyObjPtr ListIteratorKlass::repr(const PyObjPtr& obj) {
  auto iterator = obj->as<ListIterator>();
  return StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create("<list_iterator object of "), iterator->List()->str(),
       PyString::Create(" index: "),
       CreatePyInteger(iterator->CurrentIndex())->str(), PyString::Create(">")}
    )
  );
}

PyObjPtr ListReverseIteratorKlass::next(const PyObjPtr& obj) {
  auto iterator = obj->as<ListReverseIterator>();
  auto list = iterator->List();
  if ((~(iterator->CurrentIndex())) == 0U) {
    return CreateIterDone();
  }
  auto value = list->GetItem(iterator->CurrentIndex());
  iterator->Next();
  return value;
}

PyObjPtr ListReverseIteratorKlass::repr(const PyObjPtr& obj) {
  auto iterator = obj->as<ListReverseIterator>();
  return StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {PyString::Create("<list_reversed_iterator object of "),
       iterator->List()->str(), PyString::Create(" index: "),
       CreatePyInteger(iterator->CurrentIndex())->str(), PyString::Create(">")}
    )
  );
}

PyObjPtr StringIteratorKlass::next(const PyObjPtr& obj) {
  auto iterator = obj->as<StringIterator>();
  auto string = iterator->String();
  if (iterator->CurrentIndex() >= string->Length()) {
    return CreateIterDone();
  }
  auto value = string->GetItem(iterator->CurrentIndex());
  iterator->Next();
  return value;
}

PyObjPtr StringIteratorKlass::str(const PyObjPtr& obj) {
  auto iterator = obj->as<StringIterator>();
  return iterator->String()->GetItem(iterator->CurrentIndex())->str();
}

PyObjPtr DictItemIteratorKlass::next(const PyObjPtr& obj) {
  auto iterator = obj->as<DictItemIterator>();
  auto dict = iterator->Dict();
  if (iterator->CurrentIndex() >= dict->Size()) {
    return CreateIterDone();
  }
  auto value = dict->GetItem(iterator->CurrentIndex());
  iterator->Next();
  return value;
}

PyObjPtr DictItemIteratorKlass::str(const PyObjPtr& obj) {
  auto iterator = obj->as<DictItemIterator>();
  auto dict = iterator->Dict();
  auto value = dict->GetItem(iterator->CurrentIndex())->as<PyList>();
  auto result = StringConcat(
    PyList::Create<Object::PyObjPtr>(
      {value->GetItem(0)->str(), PyString::Create(": ")->as<PyString>(),
       value->GetItem(1)->str()}
    )
  );
  return result;
}

}  // namespace kaubo::Object
