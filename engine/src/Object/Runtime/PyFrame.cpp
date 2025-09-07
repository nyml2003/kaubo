#include "Object/Runtime/PyFrame.h"
#include "ByteCode/ByteCode.h"
#include "Collections/Stack.h"
#include "Collections/String/BytesHelper.h"
#include "Function/BuiltinFunction.h"
#include "Object/Container/PyDictionary.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyObject.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyFunction.h"
#include "Object/Iterator/Iterator.h"
#include "Object/Iterator/PyGenerator.h"
#include "Object/Number/PyInteger.h"
#include "Object/PySlice.h"
#include "Object/Runtime/PyCode.h"
#include "Object/Runtime/PyInst.h"
#include "Object/String/PyString.h"
#include "Runtime/VirtualMachine.h"
#include "Tools/Config/Config.h"
#include "Tools/Terminal/Terminal.h"
#include "Tools/Terminal/VerboseTerminal.h"

namespace kaubo::Object {

PyFrame::PyFrame(
  PyCodePtr code,
  PyDictPtr locals,
  PyDictPtr globals,
  PyListPtr fastLocals,
  PyFramePtr caller
)
  : PyObject(FrameKlass::Self()),
    stack(),
    programCounter(0),
    code(std::move(code)),
    locals(std::move(locals)),
    globals(std::move(globals)),
    fastLocals(std::move(fastLocals)),
    caller(std::move(caller)) {}

PyFramePtr CreateModuleEntryFrame(const PyCodePtr& code) {
  auto locals = CreatePyDict()->as<PyDictionary>();
  auto globals = locals;
  locals->Put(CreatePyString("__name__"), CreatePyString("__main__"));
  auto fastLocals = CreatePyList(code->NLocals());
  auto caller = nullptr;
  auto frame =
    std::make_shared<PyFrame>(code, locals, globals, fastLocals, caller);
  Runtime::VirtualMachine::Instance().SetFrame(frame);
  return frame;
}

PyFramePtr CreateFrameWithPyFunction(
  const PyFunctionPtr& function,
  const PyListPtr& arguments
) {
  auto code = function->Code();
  auto globals = function->Globals();
  auto locals = CreatePyDict()->as<PyDictionary>();
  Index nLocals = code->NLocals();
  for (Index i = arguments->Length(); i < nLocals; i++) {
    arguments->Append(PyNone::Instance());
  }
  auto caller = Runtime::VirtualMachine::Instance().CurrentFrame();
  auto frame =
    std::make_shared<PyFrame>(code, locals, globals, arguments, caller);
  Runtime::VirtualMachine::Instance().SetFrame(frame);
  return frame;
}

void PyFrame::SetProgramCounter(Index _pc) {
  programCounter = _pc;
}

PyCodePtr PyFrame::Code() const {
  return code;
}

Index PyFrame::ProgramCounter() const {
  return programCounter;
}

PyDictPtr PyFrame::CurrentLocals() const {
  return locals;
}

PyDictPtr PyFrame::CurrentGlobals() const {
  return globals;
}

PyListPtr PyFrame::CurrentFastLocals() const {
  return fastLocals;
}

PyListPtr PyFrame::DumpStack() const {
  return CreatePyList(stack.GetContent());
}

PyInstPtr PyFrame::Instruction() const {
  if (!isParsed) {
    ParseByteCode(code);
  }

  return code->Instructions()->GetItem(programCounter)->as<PyInst>();
}

bool PyFrame::Finished() {
  if (!isParsed) {
    ParseByteCode(code);
  }
  this->isParsed = true;
  auto size = code->Instructions()->Length();
  return programCounter >= size;
}

void PyFrame::NextProgramCounter() {
  programCounter++;
}

void ParseByteCode(const PyCodePtr& code) {
  if (code->ByteCode() == nullptr) {
    //    ConsoleTerminal::get_instance().log(
    //      "bytecode passing in memory or something wrong"
    //    );
    return;
  }
  auto bytes = code->ByteCode()->Value().CopyCodeUnits();
  Index iter = 0;
  if (static_cast<Literal>(bytes[iter]) != Literal::LIST) {
    throw std::runtime_error("Invalid insts");
  }
  iter++;
  Index size = Collections::DeserializeU64(bytes, iter);
  auto insts = CreatePyList(size);
  for (Index pcCounter = 0; pcCounter < size; pcCounter++) {
    auto byte = bytes[iter++];
    insts->SetItem(pcCounter, [byte, &iter, &bytes]() {
      switch (static_cast<ByteCode>(byte)) {
        case ByteCode::LOAD_CONST: {
          return MakeInst<ByteCode::LOAD_CONST>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::BINARY_ADD: {
          return MakeInst<ByteCode::BINARY_ADD>();
        }
        case ByteCode::BINARY_MULTIPLY: {
          return MakeInst<ByteCode::BINARY_MULTIPLY>();
        }
        case ByteCode::BINARY_SUBTRACT: {
          return MakeInst<ByteCode::BINARY_SUBTRACT>();
        }
        case ByteCode::STORE_FAST: {
          return MakeInst<ByteCode::STORE_FAST>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::LOAD_FAST: {
          return MakeInst<ByteCode::LOAD_FAST>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::COMPARE_OP: {
          auto compareOp = bytes[iter++];
          return MakeInst<ByteCode::COMPARE_OP>(static_cast<CompareOp>(compareOp
          ));
        }
        case ByteCode::POP_JUMP_IF_FALSE: {
          return MakeInst<ByteCode::POP_JUMP_IF_FALSE>(
            Collections::DeserializeI64(bytes, iter)
          );
        }
        case ByteCode::POP_JUMP_IF_TRUE: {
          return MakeInst<ByteCode::POP_JUMP_IF_TRUE>(
            Collections::DeserializeI64(bytes, iter)
          );
        }
        case ByteCode::MAKE_FUNCTION: {
          return MakeInst<ByteCode::MAKE_FUNCTION>();
        }
        case ByteCode::CALL_FUNCTION: {
          return MakeInst<ByteCode::CALL_FUNCTION>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::RETURN_VALUE: {
          return MakeInst<ByteCode::RETURN_VALUE>();
        }
        case ByteCode::LOAD_NAME: {
          return MakeInst<ByteCode::LOAD_NAME>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::STORE_NAME: {
          return MakeInst<ByteCode::STORE_NAME>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::LOAD_GLOBAL: {
          return MakeInst<ByteCode::LOAD_GLOBAL>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::POP_TOP: {
          return MakeInst<ByteCode::POP_TOP>();
        }
        case ByteCode::LOAD_ATTR: {
          return MakeInst<ByteCode::LOAD_ATTR>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::BUILD_LIST: {
          return MakeInst<ByteCode::BUILD_LIST>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::BUILD_SLICE: {
          return MakeInst<ByteCode::BUILD_SLICE>();
        }
        case ByteCode::BUILD_MAP: {
          return MakeInst<ByteCode::BUILD_MAP>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::BINARY_MATRIX_MULTIPLY: {
          return MakeInst<ByteCode::BINARY_MATRIX_MULTIPLY>();
        }
        case ByteCode::JUMP_ABSOLUTE: {
          return MakeInst<ByteCode::JUMP_ABSOLUTE>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::BINARY_SUBSCR: {
          return MakeInst<ByteCode::BINARY_SUBSCR>();
        }
        case ByteCode::STORE_SUBSCR: {
          return MakeInst<ByteCode::STORE_SUBSCR>();
        }
        case ByteCode::GET_ITER: {
          return MakeInst<ByteCode::GET_ITER>();
        }
        case ByteCode::FOR_ITER: {
          return MakeInst<ByteCode::FOR_ITER>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::LOAD_BUILD_CLASS: {
          return MakeInst<ByteCode::LOAD_BUILD_CLASS>();
        }
        case ByteCode::STORE_ATTR: {
          return MakeInst<ByteCode::STORE_ATTR>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::NOP: {
          return MakeInst<ByteCode::NOP>();
        }
        case ByteCode::UNARY_POSITIVE: {
          return MakeInst<ByteCode::UNARY_POSITIVE>();
        }
        case ByteCode::UNARY_NEGATIVE: {
          return MakeInst<ByteCode::UNARY_NEGATIVE>();
        }
        case ByteCode::UNARY_NOT: {
          return MakeInst<ByteCode::UNARY_NOT>();
        }
        case ByteCode::UNARY_INVERT: {
          return MakeInst<ByteCode::UNARY_INVERT>();
        }
        case ByteCode::BINARY_POWER: {
          return MakeInst<ByteCode::BINARY_POWER>();
        }
        case ByteCode::BINARY_MODULO: {
          return MakeInst<ByteCode::BINARY_MODULO>();
        }
        case ByteCode::BINARY_FLOOR_DIVIDE: {
          return MakeInst<ByteCode::BINARY_FLOOR_DIVIDE>();
        }
        case ByteCode::BINARY_TRUE_DIVIDE: {
          return MakeInst<ByteCode::BINARY_TRUE_DIVIDE>();
        }
        case ByteCode::BINARY_LSHIFT: {
          return MakeInst<ByteCode::BINARY_LSHIFT>();
        }
        case ByteCode::BINARY_RSHIFT: {
          return MakeInst<ByteCode::BINARY_RSHIFT>();
        }
        case ByteCode::BINARY_AND: {
          return MakeInst<ByteCode::BINARY_AND>();
        }
        case ByteCode::BINARY_XOR: {
          return MakeInst<ByteCode::BINARY_XOR>();
        }
        case ByteCode::BINARY_OR: {
          return MakeInst<ByteCode::BINARY_OR>();
        }
        case ByteCode::YIELD_VALUE: {
          return MakeInst<ByteCode::YIELD_VALUE>();
        }
        case ByteCode::JUMP_FORWARD: {
          return MakeInst<ByteCode::JUMP_FORWARD>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
        case ByteCode::STORE_GLOBAL: {
          return MakeInst<ByteCode::STORE_GLOBAL>(
            Collections::DeserializeU64(bytes, iter)
          );
        }
      }
    }());
  }
  code->SetInstructions(std::move(insts));
}

PyObjPtr FrameKlass::repr(const PyObjPtr& obj) {
  if (!obj->is(FrameKlass::Self())) {
    throw std::runtime_error("repr(): klass is not a frame");
  }
  auto frame = obj->as<PyFrame>();
  return StringConcat(CreatePyList(
    {CreatePyString("<frame object at ")->as<PyString>(),
     Function::Identity(CreatePyList({obj}))->as<PyString>(),
     CreatePyString(">")->as<PyString>()}
  ));
}

void PrintFrame(const PyFramePtr& frame) {  // NOLINT(misc-no-recursion)
  // Frame representation
  VerboseTerminal::get_instance().info(
    frame->repr()->as<PyString>()->ToCppString()
  );

  // Program Counter
  VerboseTerminal::get_instance().info("Program Counter: ");
  auto pc_repr = CreatePyInteger(frame->ProgramCounter())
                   ->repr()
                   ->as<PyString>()
                   ->ToCppString();
  VerboseTerminal::get_instance().info(pc_repr);

  // Current Instruction
  VerboseTerminal::get_instance().info("Current Instruction: ");
  auto instr_repr = frame->Instruction()->repr()->as<PyString>()->ToCppString();
  VerboseTerminal::get_instance().info(instr_repr);

  // Code
  VerboseTerminal::get_instance().info("Code:");
  VerboseTerminal::IncreaseIndent();
  PrintCode(frame->Code());
  VerboseTerminal::DecreaseIndent();

  // Stack
  // VerboseTerminal::get_instance().log("Stack(*ptr):");
  // auto stack = frame->DumpStack();
  // for (Index i = 0; i < stack->Length(); i++) {
  //   VerboseTerminal::get_instance().log(std::to_string(i) + ": ");
  //   VerboseTerminal::get_instance().log("  ");
  //   auto item = stack->GetItem(i);
  //   auto item_repr = item->repr()->as<PyString>()->ToCppString();
  //   VerboseTerminal::get_instance().log(item_repr);
  //   auto ptr = reinterpret_cast<uint64_t>(item.get());
  //   std::string ptr_str = " ( " + std::to_string(ptr) + " ) ";
  //   VerboseTerminal::get_instance().log(ptr_str);
  // }

  // Locals
  VerboseTerminal::get_instance().info("Locals:");
  auto locals_repr =
    frame->CurrentLocals()->repr()->as<PyString>()->ToCppString();
  VerboseTerminal::get_instance().info(locals_repr + "");

  // Globals
  VerboseTerminal::get_instance().info("Globals:");
  auto globals_repr =
    frame->CurrentGlobals()->repr()->as<PyString>()->ToCppString();
  VerboseTerminal::get_instance().info(globals_repr + "");

  // FastLocals
  VerboseTerminal::get_instance().info("FastLocals:");
  auto fast_locals_repr =
    frame->CurrentFastLocals()->repr()->as<PyString>()->ToCppString();
  VerboseTerminal::get_instance().info(fast_locals_repr + "");

  // Caller
  VerboseTerminal::get_instance().info("Caller: ");
  if (frame->HasCaller()) {
    VerboseTerminal::IncreaseIndent();
    PrintFrame(frame->Caller());
    VerboseTerminal::DecreaseIndent();
  } else {
    VerboseTerminal::get_instance().info("This is the top frame");
  }
  VerboseTerminal::get_instance().info("</frame>");
}
PyFramePtr PyFrame::Caller() const {
  return caller;
}

bool PyFrame::HasCaller() const {
  return caller != nullptr;
}

PyObjPtr PyFrame::Eval() {  // NOLINT(readability-function-cognitive-complexity)
  while (!Finished()) {
    auto inst = Instruction();
    auto byteCode = inst->Code();
    auto oprt = inst->Operand();
    if (Config::has("verbose")) {
      PrintFrame(shared_from_this()->as<PyFrame>());
    }
    switch (byteCode) {
      case ByteCode::LOAD_CONST: {
        auto key = std::get<Index>(oprt);
        auto value = Code()->Consts()->getitem(CreatePyInteger(key));
        stack.Push(value);
        NextProgramCounter();
        break;
      }
      case ByteCode::STORE_GLOBAL: {
        auto key = std::get<Index>(oprt);
        auto value = stack.Pop();
        globals->setitem(CreatePyInteger(key), value);
        NextProgramCounter();
        break;
      }
      case ByteCode::STORE_FAST: {
        auto index = std::get<Index>(oprt);
        auto value = stack.Pop();
        fastLocals->setitem(CreatePyInteger(index), value);
        NextProgramCounter();
        break;
      }
      case ByteCode::COMPARE_OP: {
        auto compareOp = std::get<CompareOp>(oprt);
        auto right = stack.Pop();
        auto left = stack.Pop();
        switch (compareOp) {
          case CompareOp::EQUAL: {
            stack.Push(left->eq(right));
            break;
          }
          case CompareOp::NOT_EQUAL: {
            stack.Push(left->ne(right));
            break;
          }
          case CompareOp::LESS_THAN: {
            stack.Push(left->lt(right));
            break;
          }
          case CompareOp::LESS_THAN_EQUAL: {
            stack.Push(left->le(right));
            break;
          }
          case CompareOp::GREATER_THAN: {
            auto result = left->gt(right);
            stack.Push(left->gt(right));
            break;
          }
          case CompareOp::GREATER_THAN_EQUAL: {
            stack.Push(left->ge(right));
            break;
          }
          case CompareOp::IN: {
            stack.Push(right->contains(left));
            break;
          }
          case CompareOp::NOT_IN: {
            stack.Push(Not(right->contains(left)));
            break;
          }
          case CompareOp::IS: {
            stack.Push(CreatePyBoolean(left.get() == right.get()));
            break;
          }
          case CompareOp::IS_NOT: {
            stack.Push(CreatePyBoolean(left.get() != right.get()));
            break;
          }
          default:
            throw std::runtime_error("Unknown compare operation");
        }
        NextProgramCounter();
        break;
      }
      case ByteCode::POP_JUMP_IF_FALSE: {
        auto needJump = stack.Pop();
        if (!IsTrue(needJump)) {
          SetProgramCounter(
            static_cast<Index>(
              static_cast<int64_t>(ProgramCounter()) + std::get<int64_t>(oprt)
            )
          );
        } else {
          NextProgramCounter();
        }
        break;
      }
      case ByteCode::POP_JUMP_IF_TRUE: {
        auto needJump = stack.Pop();
        if (IsTrue(needJump)) {
          SetProgramCounter(
            static_cast<Index>(
              static_cast<int64_t>(ProgramCounter()) + std::get<int64_t>(oprt)
            )
          );
        } else {
          NextProgramCounter();
        }
        break;
      }
      case ByteCode::BINARY_ADD: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->add(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_SUBTRACT: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->sub(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_MULTIPLY: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->mul(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_MATRIX_MULTIPLY: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->matmul(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_TRUE_DIVIDE: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->truediv(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_FLOOR_DIVIDE: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->floordiv(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_XOR: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->_xor_(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_AND: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->_and_(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_OR: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->_or_(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_POWER: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->pow(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_MODULO: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->mod(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_LSHIFT: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->lshift(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_RSHIFT: {
        auto right = stack.Pop();
        auto left = stack.Pop();
        stack.Push(left->rshift(right));
        NextProgramCounter();
        break;
      }
      case ByteCode::UNARY_POSITIVE: {
        auto operand = stack.Pop();
        stack.Push(operand->pos());
        NextProgramCounter();
        break;
      }
      case ByteCode::UNARY_NEGATIVE: {
        auto operand = stack.Pop();
        stack.Push(operand->neg());
        NextProgramCounter();
        break;
      }
      case ByteCode::UNARY_NOT: {
        auto operand = stack.Pop();
        stack.Push(Not(operand->boolean()));
        NextProgramCounter();
        break;
      }
      case ByteCode::UNARY_INVERT: {
        auto operand = stack.Pop();
        stack.Push(operand->invert());
        NextProgramCounter();
        break;
      }
      case ByteCode::BINARY_SUBSCR: {
        auto index = stack.Pop();
        auto obj = stack.Pop();
        stack.Push(obj->getitem(index));
        NextProgramCounter();
        break;
      }
      case ByteCode::RETURN_VALUE: {
        auto value = stack.Pop();
        return value;
      }
      case ByteCode::MAKE_FUNCTION: {
        auto name = stack.Pop();
        if (!name->is(StringKlass::Self())) {
          throw std::runtime_error("Function name must be string");
        }
        auto codeObj = stack.Pop();
        if (!codeObj->is(CodeKlass::Self())) {
          throw std::runtime_error("Function code must be code object");
        }
        auto func = CreatePyFunction(codeObj, globals);
        stack.Push(func);
        NextProgramCounter();
        break;
      }
      case ByteCode::LOAD_FAST: {
        auto index = std::get<Index>(oprt);
        auto value = fastLocals->GetItem(index);
        stack.Push(value);
        NextProgramCounter();
        break;
      }
      case ByteCode::CALL_FUNCTION: {
        auto argumentCount = std::get<Index>(oprt);
        auto argList = CreatePyList(stack.Top(argumentCount));
        auto func = stack.Pop();
        auto result = Runtime::Evaluator::InvokeCallable(func, argList);
        stack.Push(result);
        NextProgramCounter();
        break;
      }
      case ByteCode::LOAD_GLOBAL: {
        auto index = std::get<Index>(oprt);
        auto key = Code()->Names()->GetItem(index);
        bool found = false;
        PyObjPtr value = CreatePyNone();
        if (globals->Contains(key)) {
          found = true;
          value = globals->getitem(key);
        }
        if (!found &&
            IsTrue(
              Runtime::VirtualMachine::Instance().Builtins()->contains(key)
            )) {
          found = true;
          value = Runtime::VirtualMachine::Instance().Builtins()->getitem(key);
        }
        if (!found) {
          auto errorMessage = StringConcat(CreatePyList(
            {CreatePyString("NameError: name '"), key,
             CreatePyString("' is not defined")}
          ));
          throw std::runtime_error(errorMessage->as<PyString>()->ToCppString());
        }
        stack.Push(value);
        NextProgramCounter();
        break;
      }
      case ByteCode::STORE_NAME: {
        auto index = std::get<Index>(oprt);
        auto key = Code()->Names()->GetItem(index);
        auto value = stack.Pop();
        locals->setitem(key, value);
        NextProgramCounter();
        break;
      }
      case ByteCode::LOAD_NAME: {
        auto index = std::get<Index>(oprt);
        auto key = Code()->Names()->GetItem(index);
        // LEGB rule
        // local -> enclosing -> global -> built-in
        bool found = false;
        PyObjPtr value = CreatePyNone();
        if (IsTrue(locals->contains(key))) {
          found = true;
          value = locals->getitem(key);
        }
        if (!found && IsTrue(globals->contains(key))) {
          found = true;
          value = globals->getitem(key);
        }
        if (!found &&
            IsTrue(
              Runtime::VirtualMachine::Instance().Builtins()->contains(key)
            )) {
          found = true;
          value = Runtime::VirtualMachine::Instance().Builtins()->getitem(key);
        }
        if (!found) {
          throw std::runtime_error(
            "NameError: name '" + key->as<PyString>()->ToCppString() +
            "' is not defined"
          );
        }
        stack.Push(value);
        NextProgramCounter();
        break;
      }
      case ByteCode::POP_TOP: {
        stack.Pop();
        NextProgramCounter();
        break;
      }
      case ByteCode::LOAD_ATTR: {
        auto index = std::get<Index>(oprt);
        auto key = Code()->Names()->GetItem(index);
        auto obj = stack.Pop();
        auto value = obj->getattr(key);
        if (value == nullptr) {
          ConsoleTerminal::get_instance().debug("object attributes: ");
          obj->Attributes()->str()->as<PyString>()->Print();
          ConsoleTerminal::get_instance().debug("class attributes: ");
          obj->Klass()->Attributes()->str()->as<PyString>()->Print();
          ConsoleTerminal::get_instance().debug("mro: ");
          obj->Klass()->Mro()->str()->as<PyString>()->Print();
          for (Index i = 0; i < obj->Klass()->Mro()->Length(); i++) {
            auto mro = obj->Klass()->Mro()->GetItem(i)->as<PyType>();
            mro->Owner()->Name()->str()->as<PyString>()->Print();
            mro->str()->as<PyString>()->Print();
          }
          throw std::runtime_error(
            "AttributeError: '" +
            obj->Klass()->Name()->as<PyString>()->ToCppString() +
            "' object has no attribute '" + key->as<PyString>()->ToCppString() +
            "'"
          );
        }
        stack.Push(value);
        NextProgramCounter();
        break;
      }
      case ByteCode::BUILD_LIST: {
        auto size = std::get<Index>(oprt);
        Collections::List<PyObjPtr> elements(size);
        for (Index i = 0; i < size; i++) {
          elements.Push(stack.Pop());
        }
        auto list = CreatePyList(elements);
        stack.Push(list);
        NextProgramCounter();
        break;
      }
      case ByteCode::BUILD_SLICE: {
        auto step = stack.Pop();
        auto end = stack.Pop();
        auto start = stack.Pop();
        auto slice = CreatePySlice(start, end, step);
        stack.Push(slice);
        NextProgramCounter();
        break;
      }
      case ByteCode::JUMP_ABSOLUTE: {
        SetProgramCounter(std::get<Index>(oprt));
        break;
      }
      case ByteCode::STORE_SUBSCR: {
        auto index = stack.Pop();
        auto obj = stack.Pop();
        auto value = stack.Pop();
        obj->setitem(index, value);
        NextProgramCounter();
        break;
      }
      case ByteCode::GET_ITER: {
        auto obj = stack.Pop();
        auto iter = obj->iter();
        stack.Push(iter);
        NextProgramCounter();
        break;
      }
      case ByteCode::FOR_ITER: {
        auto iter = stack.Pop();
        auto value = iter->next();
        if (value->is(Object::IterDoneKlass::Self())) {
          SetProgramCounter(
            static_cast<Index>(ProgramCounter() + std::get<uint64_t>(oprt))
          );
        } else {
          stack.Push(iter);
          stack.Push(value);
          NextProgramCounter();
        }
        break;
      }
      case ByteCode::LOAD_BUILD_CLASS: {
        stack.Push(
          Runtime::VirtualMachine::Instance().Builtins()->getitem(
            CreatePyString("__build_class__")
          )
        );
        NextProgramCounter();
        break;
      }
      case ByteCode::STORE_ATTR: {
        auto index = std::get<Index>(oprt);
        auto key = Code()->Names()->GetItem(index);
        auto obj = stack.Pop();
        auto value = stack.Pop();
        obj->setattr(key, value);
        NextProgramCounter();
        break;
      }
      case ByteCode::NOP: {
        NextProgramCounter();
        break;
      }
      case ByteCode::YIELD_VALUE: {
        NextProgramCounter();
        return CreatePyGenerator(shared_from_this()->as<PyFrame>());
      }
      case ByteCode::JUMP_FORWARD: {
        SetProgramCounter(programCounter + std::get<Index>(oprt));
        break;
      }
      case ByteCode::BUILD_MAP: {
        auto size = std::get<Index>(oprt);
        auto map = CreatePyDict();
        for (Index i = 0; i < size; i++) {
          auto value = stack.Pop();
          auto key = stack.Pop();
          map->setitem(key, value);
        }
        stack.Push(map);
        NextProgramCounter();
        break;
      }
    }
  }
  return CreatePyNone();
}

PyObjPtr PyFrame::EvalAndDestroy() {
  auto result = Eval();
  Runtime::VirtualMachine::Instance().BackToParentFrame();
  return result;
}

}  // namespace kaubo::Object
