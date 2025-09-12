#include "IR/Expression/Slice.h"
#include "Object/Core/PyNone.h"
#include "Object/Iterator/IteratorHelper.h"
namespace kaubo::IR {
Object::PyObjPtr SliceKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto list = obj->as<IR::Slice>();
  auto elements = list->Elements();
  Object::ForEach(elements, [&codeList](const Object::PyObjPtr& element) {
    element->as<INode>()->emit(codeList);
  });
  auto code = GetCodeFromList(codeList, list);
  code->BuildSlice();
  return Object::PyNone::Create();
}

Object::PyObjPtr SliceKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto list = obj->as<IR::Slice>();
  auto elements = list->Elements();
  Object::ForEach(elements, [&codeList](const Object::PyObjPtr& element) {
    element->as<INode>()->visit(codeList);
  });
  return Object::PyNone::Create();
}

Object::PyObjPtr SliceKlass::print(const Object::PyObjPtr& obj) {
  auto list = obj->as<IR::Slice>();
  auto elements = list->Elements();
  PrintNode(list, Object::PyString::Create("Slice"));
  Object::ForEach(elements, [&](const Object::PyObjPtr& element) {
    element->as<INode>()->print();
    PrintEdge(list, element);
  });
  return Object::PyNone::Create();
}

}  // namespace kaubo::IR