#include "IR/Expression/Map.h"
#include "Object/Core/PyNone.h"
#include "Object/Iterator/IteratorHelper.h"
namespace kaubo::IR {
Object::PyObjPtr
MapKlass::emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) {
  auto map = obj->as<IR::Map>();
  auto keys = map->Keys();
  auto values = map->Values();
  Index size = keys->Length();
  auto code = GetCodeFromList(codeList, map);
  for (Index i = size - 1; ~i; --i) {
    keys->GetItem(i)->as<INode>()->emit(codeList);
    values->GetItem(i)->as<INode>()->emit(codeList);
  }
  code->BuildMap(size);
  return Object::CreatePyNone();
}

Object::PyObjPtr
MapKlass::visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) {
  auto map = obj->as<IR::Map>();
  auto keys = map->Keys();
  auto values = map->Values();
  Object::ForEach(keys, [&codeList](const Object::PyObjPtr& element) {
    element->as<INode>()->visit(codeList);
  });
  Object::ForEach(values, [&codeList](const Object::PyObjPtr& element) {
    element->as<INode>()->visit(codeList);
  });
  return Object::CreatePyNone();
}

Object::PyObjPtr MapKlass::print(const Object::PyObjPtr& obj) {
  auto map = obj->as<Map>();
  auto keys = map->Keys();
  auto values = map->Values();
  PrintNode(map, Object::PyString::Create("Map"));
  Object::ForEach(keys, [&](const Object::PyObjPtr& element) {
    element->as<INode>()->print();
    PrintEdge(map, element, Object::PyString::Create("key"));
  });
  Object::ForEach(values, [&](const Object::PyObjPtr& element) {
    element->as<INode>()->print();
    PrintEdge(map, element, Object::PyString::Create("value"));
  });
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR