#pragma once
#include <cstdint>
#include "Object/Core/PyObject.h"
#include "Object/Runtime/PyCode.h"
namespace kaubo::IR {

enum class STOREORLOAD : uint8_t {
  STORE,
  LOAD,
};

class INodeTrait {
 public:
  virtual Object::PyObjPtr
  visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) = 0;

  virtual Object::PyObjPtr
  emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList) = 0;

  virtual Object::PyObjPtr print(const Object::PyObjPtr& obj) = 0;

  virtual ~INodeTrait() = default;

  INodeTrait() = default;

  INodeTrait(const INodeTrait&) = default;

  INodeTrait& operator=(const INodeTrait&) = default;

  INodeTrait(INodeTrait&&) = default;

  INodeTrait& operator=(INodeTrait&&) = default;
};

class INode;

using INodePtr = std::shared_ptr<INode>;

class INode : public Object::PyObject {
 public:
  explicit INode(Object::KlassPtr klass, INodePtr parent)
    : PyObject(klass), parent(std::move(parent)) {}

  [[nodiscard]] INodePtr Parent() const { return parent; }

  /**
   * @brief
   * 遍历AST树，在当前INode节点所属的PyCode对象中注册常量表(consts)，变量表(names)
   */
  virtual Object::PyObjPtr visit(const Object::PyObjPtr& codeList) {
    return dynamic_cast<INodeTrait*>(Klass())->visit(
      shared_from_this(), codeList
    );
  }

  /**
   * @brief
   * 遍历AST树，在当前INode节点所属的PyCode对象中生成字节码
   */
  virtual Object::PyObjPtr emit(const Object::PyObjPtr& codeList) {
    return dynamic_cast<INodeTrait*>(Klass())->emit(
      shared_from_this(), codeList
    );
  }

  /**
   * @brief
   * 遍历AST树，在当前INode节点所属的PyCode对象中打印AST树
   */
  virtual Object::PyObjPtr print() {
    return dynamic_cast<INodeTrait*>(Klass())->print(shared_from_this());
  }

 private:
  INodePtr parent;  // 保存父结点在codeList中的索引
};

Object::PyCodePtr
GetCodeFromList(const Object::PyObjPtr& codeList, const INodePtr& node);

void PrintNode(const Object::PyObjPtr& node, const Object::PyStrPtr& text);
void PrintEdge(
  const Object::PyObjPtr& parent,
  const Object::PyObjPtr& child,
  const Object::PyStrPtr& text = nullptr
);
}  // namespace kaubo::IR
