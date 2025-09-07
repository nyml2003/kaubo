#pragma once

#include "Object/Core/PyObject.h"

namespace kaubo::Object {

PyObjPtr Transpose(const PyObjPtr& args);

PyObjPtr Array(const PyObjPtr& args);

PyObjPtr Eye(const PyObjPtr& args);

PyObjPtr Zeros(const PyObjPtr& args);

PyObjPtr Ones(const PyObjPtr& args);

PyObjPtr Diagnostic(const PyObjPtr& args);

PyObjPtr Reshape(const PyObjPtr& args);

PyObjPtr Shape(const PyObjPtr& args);

PyObjPtr Concatenate(const PyObjPtr& args);

PyObjPtr Ravel(const PyObjPtr& args);
}  // namespace kaubo::Object
