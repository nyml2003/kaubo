#include "IR/MemberAccess.h"
#include "IR/Expression/Atom.h"
#include "Object/Core/PyNone.h"

namespace kaubo::IR {

Object::PyObjPtr MemberAccessKlass::visit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto memberAccess = obj->as<MemberAccess>();
  auto object = memberAccess->Obj();
  object->visit(codeList);
  auto member = memberAccess->Member();
  auto code = GetCodeFromList(codeList, memberAccess);
  code->RegisterName(member);
  return Object::PyNone::Create();
}

Object::PyObjPtr MemberAccessKlass::emit(
  const Object::PyObjPtr& obj,
  const Object::PyObjPtr& codeList
) {
  auto memberAccess = obj->as<MemberAccess>();
  auto object = memberAccess->Obj();
  object->emit(codeList);
  auto member = memberAccess->Member();
  auto code = GetCodeFromList(codeList, memberAccess);
  if (memberAccess->Mode() == STOREORLOAD::STORE) {
    code->StoreAttr(member);
  }
  if (memberAccess->Mode() == STOREORLOAD::LOAD) {
    code->LoadAttr(member);
  }
  return Object::PyNone::Create();
}

Object::PyObjPtr MemberAccessKlass::print(const Object::PyObjPtr& obj) {
  auto memberAccess = obj->as<MemberAccess>();
  auto object = memberAccess->Obj();
  auto member = CreateAtom(memberAccess->Member(), nullptr);
  PrintNode(memberAccess, Object::PyString::Create("MemberAccess"));
  object->print();
  member->print();
  PrintEdge(memberAccess, object, Object::PyString::Create("object"));
  PrintEdge(memberAccess, member, Object::PyString::Create("member"));
  return Object::PyNone::Create();
}

}  // namespace kaubo::IR
