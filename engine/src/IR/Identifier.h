#pragma once

#include "IR/INode.h"
#include "Object/String/PyString.h"

namespace kaubo::IR {

class IdentifierKlass : public INodeTrait,
                        public Object::KlassBase<IdentifierKlass> {
 public:
  IdentifierKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_identifier"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;
  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class Identifier : public INode {
 public:
  explicit Identifier(Object::PyStrPtr _name, INodePtr parent)
    : INode(IdentifierKlass::Self(), std::move(parent)),
      name(std::move(_name)) {}

  [[nodiscard]] Object::PyStrPtr Name() const { return name; }
  void SetStoreMode() { mode = STOREORLOAD::STORE; }
  STOREORLOAD Mode() const { return mode; }
  [[nodiscard]] static Object::PyListPtr Builtins() {
    static Object::PyListPtr builtins = Object::CreatePyList({
      Object::CreatePyString("print"),
      Object::CreatePyString("reshape"),
      Object::CreatePyString("Reshape"),
      Object::CreatePyString("len"),
      Object::CreatePyString("__name__"),
      Object::CreatePyString("randint"),
      Object::CreatePyString("sleep"),
      Object::CreatePyString("input"),
      Object::CreatePyString("next"),
      Object::CreatePyString("int"),
      Object::CreatePyString("float"),
      Object::CreatePyString("str"),
      Object::CreatePyString("list"),
      Object::CreatePyString("object"),
      Object::CreatePyString("type"),
      Object::CreatePyString("dict"),
      Object::CreatePyString("slice"),
      Object::CreatePyString("repr"),
      Object::CreatePyString("bool"),
      Object::CreatePyString("whoami"),
      Object::CreatePyString("Array"),
      Object::CreatePyString("Eye"),
      Object::CreatePyString("Zeros"),
      Object::CreatePyString("Ones"),
      Object::CreatePyString("Diag"),
      Object::CreatePyString("Transpose"),
      Object::CreatePyString("Shape"),
      Object::CreatePyString("Concatenate"),
      Object::CreatePyString("Ravel"),
      Object::CreatePyString("Normal"),
      Object::CreatePyString("Shuffle"),
      Object::CreatePyString("LogisticLoss"),
      Object::CreatePyString("LogisticLossDerivative"),
      Object::CreatePyString("Sum"),
      Object::CreatePyString("Log"),
      Object::CreatePyString("Softmax"),
      Object::CreatePyString("Max"),
      Object::CreatePyString("ArgMax"),
      Object::CreatePyString("id"),
      Object::CreatePyString("hash"),
      Object::CreatePyString("time"),
      Object::CreatePyString("range"),
      Object::CreatePyString("iter"),
      Object::CreatePyString("Promise"),
      Object::CreatePyString("readFile")
      // Object::CreatePyString("co")}
    });
    return builtins;
  }

 private:
  Object::PyStrPtr name;
  STOREORLOAD mode = STOREORLOAD::LOAD;
  Object::PyListPtr builtins;
};

using IdentifierPtr = std::shared_ptr<Identifier>;

inline INodePtr
CreateIdentifier(const Object::PyStrPtr& name, const INodePtr& parent) {
  return std::make_shared<Identifier>(name, parent);
}

enum class IdentifierRegistry {
  UNREGISTERED,
  LOCAL_VARNAME,
  LOCAL_NAME,
  GLOBAL_NAME,
  BUILTIN
};

IdentifierRegistry GetIdentifierRegistry(
  const Object::PyStrPtr& name,
  const Object::PyCodePtr& current,
  const Object::PyCodePtr& global,
  const Object::PyListPtr& builtins
);

}  // namespace kaubo::IR