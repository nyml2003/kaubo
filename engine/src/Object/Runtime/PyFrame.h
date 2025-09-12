#pragma once

#include "Collections/Stack.h"
#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/CoreHelper.h"
#include "Object/Core/Klass.h"
#include "Object/Core/PyObject.h"
#include "Object/Function/PyFunction.h"
#include "Object/Object.h"
#include "Object/Runtime/PyCode.h"
#include "Object/Runtime/PyInst.h"

namespace kaubo::Object {
class PyFrame;
using PyFramePtr = std::shared_ptr<PyFrame>;

class PyFrame : public PyObject {
 private:
  Collections::Stack<PyObjPtr> stack;
  Index programCounter;

  PyCodePtr code;
  PyDictPtr locals;
  PyDictPtr globals;
  PyListPtr fastLocals;
  PyFramePtr caller;
  bool isParsed = false;

 public:
  explicit PyFrame(
    PyCodePtr code,
    PyDictPtr locals,
    PyDictPtr globals,
    PyListPtr fastLocals,
    PyFramePtr caller
  );

  void SetProgramCounter(Index _pc);

  [[nodiscard]] PyCodePtr Code() const;

  [[nodiscard]] Index ProgramCounter() const;

  [[nodiscard]] PyFramePtr Caller() const;

  [[nodiscard]] PyInstPtr Instruction() const;

  PyListPtr DumpStack() const;

  PyObjPtr StackTop() const { return stack.Top(); }

  void StackPush(const PyObjPtr& obj) { stack.Push(obj); }

  PyObjPtr StackPop() { return stack.Pop(); }

  PyDictPtr CurrentLocals() const;

  PyDictPtr CurrentGlobals() const;

  PyListPtr CurrentFastLocals() const;

  bool Finished();

  void NextProgramCounter();

  bool HasCaller() const;

  [[nodiscard]] PyObjPtr Eval();

  [[nodiscard]] PyObjPtr EvalAndDestroy();
};

using PyFramePtr = std::shared_ptr<PyFrame>;

PyFramePtr CreateModuleEntryFrame(const PyCodePtr& code);

PyFramePtr CreateFrameWithPyFunction(
  const PyFunctionPtr& function,
  const PyListPtr& arguments
);

class FrameKlass : public KlassBase<FrameKlass> {
 public:
  explicit FrameKlass() = default;

  void Initialize() override {
    if (this->IsInitialized()) {
      return;
    }
    LoadClass(PyString::Create("frame")->as<PyString>(), Self());
    ConfigureBasicAttributes(Self());
    this->SetInitialized();
  }

  PyObjPtr repr(const PyObjPtr& obj) override;
};

void ParseByteCode(const PyCodePtr& code);
void PrintFrame(const PyFramePtr& frame);
inline PyFramePtr CreatePyFrame(
  const PyCodePtr& code,
  const PyDictPtr& locals,
  const PyDictPtr& globals,
  const PyListPtr& fastLocals,
  const PyFramePtr& caller
) {
  return std::make_shared<PyFrame>(code, locals, globals, fastLocals, caller);
}
}  // namespace kaubo::Object
