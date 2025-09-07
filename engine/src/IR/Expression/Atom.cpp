#include "IR/Expression/Atom.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"

namespace kaubo::IR {

Object::PyObjPtr
AtomKlass::emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) {
  auto atom = obj->as<Atom>();
  auto code = GetCodeFromList(codeList, atom);
  auto object = atom->Obj();
  if (std::dynamic_pointer_cast<INode>(object) == nullptr) {
    code->LoadConst(object);
    return Object::CreatePyNone();
  }
  throw std::runtime_error("AtomKlass::emit(): unsupported object type");
}

Object::PyObjPtr AtomKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto atom = obj->as<Atom>();
  auto code = GetCodeFromList(codeList, atom);
  auto object = atom->Obj();
  if (std::dynamic_pointer_cast<INode>(object) == nullptr) {
    code->RegisterConst(object);
    return Object::CreatePyNone();
  }
  throw std::runtime_error("AtomKlass::visit(): unsupported object type");
}

Object::PyObjPtr AtomKlass::print(const Object::PyObjPtr& obj) {
  auto atom = obj->as<Atom>();
  PrintNode(
    atom, Object::StringConcat(
            Object::CreatePyList(
              {Object::CreatePyString("Atom: "), atom->Obj()->str()}
            )
          )
            ->as<Object::PyString>()
  );
  return Object::CreatePyNone();
}

}  // namespace kaubo::IR