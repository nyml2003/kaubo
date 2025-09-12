#pragma once
#include "IR/INode.h"

namespace kaubo::IR {

class MemberAccessKlass : public INodeTrait,
                          public Object::KlassBase<MemberAccessKlass> {
 public:
  MemberAccessKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_memberaccess"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class MemberAccess : public INode {
 public:
  MemberAccess(INodePtr obj, Object::PyStrPtr member, const INodePtr& parent)
    : INode(MemberAccessKlass::Self(), parent),
      obj(std::move(obj)),
      member(std::move(member)) {}

  [[nodiscard]] INodePtr Obj() const { return obj; }

  [[nodiscard]] Object::PyStrPtr Member() const { return member; }

  [[nodiscard]] STOREORLOAD Mode() const { return mode; }

  void SetStoreMode() { mode = STOREORLOAD::STORE; }

 private:
  INodePtr obj;
  Object::PyStrPtr member;
  STOREORLOAD mode = STOREORLOAD::LOAD;
};

using MemberAccessPtr = std::shared_ptr<MemberAccess>;

inline INodePtr CreateMemberAccess(
  const INodePtr& obj,
  const Object::PyStrPtr& member,
  const INodePtr& parent
) {
  return std::make_shared<MemberAccess>(obj, member, parent);
}

}  // namespace kaubo::IR
