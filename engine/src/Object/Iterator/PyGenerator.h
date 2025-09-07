#pragma once
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Iterator/Iterator.h"
#include "Object/Runtime/PyFrame.h"
#include "Runtime/VirtualMachine.h"

namespace kaubo::Object {
class PyGenerator;
class GeneratorKlass : public KlassBase<GeneratorKlass> {
 public:
  explicit GeneratorKlass() = default;

  void Initialize() override;

  PyObjPtr iter(const PyObjPtr& obj) override { return obj; }
  PyObjPtr next(const PyObjPtr& obj) override;
};

class PyGenerator;
using PyGeneratorPtr = std::shared_ptr<PyGenerator>;

class PyGenerator : public PyObject {
 private:
  PyFramePtr frame;
  bool isExhausted{};
  std::function<PyObjPtr(const PyGeneratorPtr&)> func;

 public:
  explicit PyGenerator(PyFramePtr _frame)
    : PyObject(GeneratorKlass::Self()), frame(std::move(_frame)) {
    func = [](const PyGeneratorPtr& self) -> PyObjPtr {
      auto lastFrame = Runtime::VirtualMachine::Instance().CurrentFrame();
      Runtime::VirtualMachine::Instance().SetFrame(self->frame);
      auto newGenerator = self->frame->Eval();  // 执行到YIELD_VALUE
      Runtime::VirtualMachine::Instance().SetFrame(lastFrame);
      if (!newGenerator->is(GeneratorKlass::Self())) {
        // 说明是return
        self->isExhausted = true;
        return CreateIterDone();
      }
      return self->frame->StackPop();
    };
  }

  explicit PyGenerator(std::function<PyObjPtr(const PyGeneratorPtr&)> _func)
    : PyObject(GeneratorKlass::Self()),

      func(std::move(_func)) {}
  [[nodiscard]] PyFramePtr Frame() const { return frame; }
  [[nodiscard]] bool IsExhausted() const { return isExhausted; }
  void SetExhausted() { isExhausted = true; }
  PyObjPtr Send(const PyObjPtr& value) {
    if (isExhausted) {
      return CreateIterDone();
    }
    if (frame) {
      frame->StackPush(value);  // 先压栈
    }

    return func(shared_from_this()->as<PyGenerator>());
  }

  PyObjPtr Next() { return Send(CreatePyNone()); }
};

inline PyObjPtr CreatePyGenerator(const PyFramePtr& frame) {
  return std::make_shared<PyGenerator>(frame);
}

inline PyObjPtr CreatePyGenerator(
  const std::function<PyObjPtr(const PyGeneratorPtr&)>& func
) {
  return std::make_shared<PyGenerator>(func);
}

}  // namespace kaubo::Object
