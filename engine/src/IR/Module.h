#pragma once

#include "IR/INode.h"

namespace kaubo::IR {

class ModuleKlass : public INodeTrait, public Object::KlassBase<ModuleKlass> {
 public:
  explicit ModuleKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_module"), Self());
    this->SetInitialized();
  }
  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class Module : public INode {
 public:
  explicit Module(Object::PyListPtr body, Object::PyStrPtr name)
    : INode(ModuleKlass::Self(), nullptr),
      body(std::move(body)),
      name(std::move(name)) {}

  Object::PyListPtr Body() const { return body; }

  Object::PyStrPtr Name() const { return name; }

  [[nodiscard]] Index CodeIndex() const { return codeIndex; }

  void SetCodeIndex(const Index _codeIndex) { this->codeIndex = _codeIndex; }

  void SetBody(const Object::PyListPtr& new_body) { this->body = new_body; }

 private:
  Object::PyListPtr body;
  Object::PyStrPtr name;
  Index codeIndex{};  // 保存当前FuncDef对应的PyCode对象在codeList中的索引
};

using ModulePtr = std::shared_ptr<Module>;

inline INodePtr
CreateModule(const Object::PyListPtr& body, const Object::PyStrPtr& name) {
  return std::make_shared<Module>(body, name);
}

}  // namespace kaubo::IR
