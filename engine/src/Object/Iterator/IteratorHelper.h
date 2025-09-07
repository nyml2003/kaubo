#pragma once

#include <functional>
#include "Object/Object.h"

namespace kaubo::Object {
void ForEach(
  const PyObjPtr& obj,
  const std::function<void(const PyObjPtr& value)>& func
);
PyObjPtr Map(
  const PyObjPtr& iterable,
  const std::function<PyObjPtr(const PyObjPtr& value)>& func
);
}  // namespace kaubo::Object
