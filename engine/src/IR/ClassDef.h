#pragma once

#include "IR/INode.h"
#include "Object/Object.h"

namespace kaubo::IR {

class ClassDefKlass : public INodeTrait,
                      public Object::KlassBase<ClassDefKlass> {
 public:
  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::CreatePyString("ast_classdef"), Self());
    this->SetInitialized();
  }

  Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) override;

  Object::PyObjPtr print(const Object::PyObjPtr& obj) override;
};

class ClassDef : public INode {
 public:
  explicit ClassDef(
    Object::PyStrPtr name,
    INodePtr bases,
    const INodePtr& parent
  );

  [[nodiscard]] Object::PyStrPtr Name() const { return name; }

  [[nodiscard]] INodePtr Bases() const { return bases; }

  [[nodiscard]] Object::PyListPtr Body() const { return body; }

  [[nodiscard]] Object::PyListPtr Parents() const { return parents; }

  [[nodiscard]] Index CodeIndex() const { return codeIndex; }

  void SetCodeIndex(const Index _codeIndex) { this->codeIndex = _codeIndex; }

  void SetBody(const Object::PyListPtr& new_body) { body = new_body; }

 private:
  Object::PyStrPtr name;   // 类名
  Object::PyListPtr body;  // 类的主体
  INodePtr bases;          // 保存当前ClassDef的基类, 是一个Identifier对象的列表
  Object::PyListPtr
    parents;  // 保存整个调用链上的ClassDef,Module和FuncDef所对应的PyCode对象
  Index codeIndex;  // 保存当前ClassDef对应的PyCode对象在codeList中的索引
};

inline INodePtr CreateClassDef(
  const Object::PyStrPtr& name,
  const INodePtr& bases,
  const INodePtr& parent
) {
  return std::make_shared<ClassDef>(name, bases, parent);
}

}  // namespace kaubo::IR
