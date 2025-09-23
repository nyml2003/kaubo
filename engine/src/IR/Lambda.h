#pragma once
#include "IR/INode.h"

namespace kaubo::IR {

class LambdaKlass : public INodeTrait, public Object::KlassBase<LambdaKlass> {
 public:
  explicit LambdaKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    InitKlass(Object::PyString::Create("ast_Lambda"), Self());
    this->SetInitialized();
  }

  auto visit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList)
    -> Object::PyObjPtr override;

  auto emit(const Object::PyObjPtr& obj, const Object::PyObjPtr& codeList)
    -> Object::PyObjPtr override;

  auto print(const Object::PyObjPtr& obj) -> Object::PyObjPtr override;
};

class Lambda : public INode {
 public:
  explicit Lambda(
    Object::PyListPtr parameters,
    Object::PyListPtr body,
    const INodePtr& parent
  );

  [[nodiscard]] auto Parameters() const -> Object::PyListPtr {
    return parameters;
  }

  [[nodiscard]] auto Body() const -> Object::PyListPtr { return body; }

  [[nodiscard]] auto Parents() const -> Object::PyListPtr { return parents; }

  [[nodiscard]] auto CodeIndex() const -> Index { return codeIndex; }

  void SetParents(const Object::PyListPtr& _parents) { parents = _parents; }

  void SetCodeIndex(const Index _codeIndex) { this->codeIndex = _codeIndex; }

  void SetBody(const Object::PyListPtr& _body) { this->body = _body; }

 private:
  Object::PyListPtr body;
  Object::PyListPtr parameters;
  Object::PyListPtr parents;  // 保存整个调用链上的PyCode对象
  Index codeIndex;  // 保存当前Lambda对应的PyCode对象在codeList中的索引
};

inline auto CreateLambda(
  const Object::PyListPtr& parameters,
  const Object::PyListPtr& body,
  const INodePtr& parent
) -> INodePtr {
  return std::make_shared<Lambda>(parameters, body, parent);
}
}  // namespace kaubo::IR
