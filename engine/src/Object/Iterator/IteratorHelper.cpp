#include "Object/Iterator/IteratorHelper.h"
#include "Object/Iterator/Iterator.h"

namespace kaubo::Object {
void ForEach(
  const PyObjPtr& obj,
  const std::function<void(const PyObjPtr& value)>& func
) {
  auto iter = obj->iter();
  auto value = iter->next();
  while (!value->is(IterDoneKlass::Self())) {
    func(value);
    value = iter->next();
  }
}

PyObjPtr Map(
  const PyObjPtr& iterable,
  const std::function<PyObjPtr(const PyObjPtr& value)>& func
) {
  auto iter = iterable->iter();
  auto value = iter->next();
  Collections::List<PyObjPtr> result;
  while (!value->is(IterDoneKlass::Self())) {
    result.Push(func(value));
    value = iter->next();
  }
  return CreatePyList(result);
}

}  // namespace kaubo::Object