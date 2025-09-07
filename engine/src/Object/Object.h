#pragma once

#include <gsl/gsl>
#include <memory>

namespace kaubo::Object {

class Klass;
using KlassPtr = gsl::owner<Klass*>;

class PyObject;
using PyObjPtr = std::shared_ptr<PyObject>;
class PyString;
using PyStrPtr = std::shared_ptr<PyString>;
class PyList;
using PyListPtr = std::shared_ptr<PyList>;
class PyDictionary;
using PyDictPtr = std::shared_ptr<PyDictionary>;
class PyType;
using PyTypePtr = std::shared_ptr<PyType>;
class PyInteger;
using PyIntPtr = std::shared_ptr<PyInteger>;

}  // namespace kaubo::Object
