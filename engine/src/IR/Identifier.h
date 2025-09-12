#pragma once

#include <cstdint>
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
    InitKlass(Object::PyString::Create("ast_identifier"), Self());
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
    static Object::PyListPtr builtins =
      Object::PyList::Create<Object::PyObjPtr>({
        Object::PyString::Create("print"),
        Object::PyString::Create("reshape"),
        Object::PyString::Create("Reshape"),
        Object::PyString::Create("len"),
        Object::PyString::Create("__name__"),
        Object::PyString::Create("randint"),
        Object::PyString::Create("sleep"),
        Object::PyString::Create("input"),
        Object::PyString::Create("next"),
        Object::PyString::Create("int"),
        Object::PyString::Create("float"),
        Object::PyString::Create("str"),
        Object::PyString::Create("list"),
        Object::PyString::Create("object"),
        Object::PyString::Create("type"),
        Object::PyString::Create("dict"),
        Object::PyString::Create("slice"),
        Object::PyString::Create("repr"),
        Object::PyString::Create("bool"),
        Object::PyString::Create("whoami"),
        Object::PyString::Create("Array"),
        Object::PyString::Create("Eye"),
        Object::PyString::Create("Zeros"),
        Object::PyString::Create("Ones"),
        Object::PyString::Create("Diag"),
        Object::PyString::Create("Transpose"),
        Object::PyString::Create("Shape"),
        Object::PyString::Create("Concatenate"),
        Object::PyString::Create("Ravel"),
        Object::PyString::Create("Normal"),
        Object::PyString::Create("Shuffle"),
        Object::PyString::Create("LogisticLoss"),
        Object::PyString::Create("LogisticLossDerivative"),
        Object::PyString::Create("Sum"),
        Object::PyString::Create("Log"),
        Object::PyString::Create("Softmax"),
        Object::PyString::Create("Max"),
        Object::PyString::Create("ArgMax"),
        Object::PyString::Create("id"),
        Object::PyString::Create("hash"),
        Object::PyString::Create("time"),
        Object::PyString::Create("range"),
        Object::PyString::Create("iter"),
        Object::PyString::Create("Promise"),
        Object::PyString::Create("readFile")
        // Object::PyString::Create("co")}
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

enum class IdentifierRegistry : uint8_t {
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