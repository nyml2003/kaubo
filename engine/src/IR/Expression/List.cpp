#include "IR/Expression/List.h"
#include "Object/Core/PyNone.h"
#include "Object/Iterator/IteratorHelper.h"
namespace kaubo::IR {
Object::PyObjPtr
ListKlass::emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) {
  auto list = obj->as<List>();
  auto elements = list->Elements();
  Object::ForEach(
    elements->reversed(),
    [&codeList](const Object::PyObjPtr& element) {
      element->as<INode>()->emit(codeList);
    }
  );
  auto code = GetCodeFromList(codeList, list);
  code->BuildList(elements->Length());
  return Object::CreatePyNone();
}

Object::PyObjPtr ListKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto list = obj->as<List>();
  auto elements = list->Elements();
  Object::ForEach(elements, [&codeList](const Object::PyObjPtr& element) {
    element->as<INode>()->visit(codeList);
  });
  return Object::CreatePyNone();
}

Object::PyObjPtr ListKlass::print(const Object::PyObjPtr& obj) {
  auto list = obj->as<List>();
  auto elements = list->Elements();
  PrintNode(list, Object::CreatePyString("List"));
  Object::ForEach(elements, [&](const Object::PyObjPtr& element) {
    element->as<INode>()->print();
    PrintEdge(list, element);
  });
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR