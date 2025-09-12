#include "Function/BuiltinFunction.h"
#include "Collections/Integer/IntegerHelper.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Core/PyPromise.h"
#include "Object/Core/PyType.h"
#include "Object/Function/PyFunction.h"
#include "Object/Function/PyNativeFunction.h"
#include "Object/Iterator/PyGenerator.h"
#include "Object/Matrix/PyMatrix.h"
#include "Object/Number/PyFloat.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Runtime/VirtualMachine.h"
#include "Tools/Terminal/Terminal.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>
#include <thread>

namespace kaubo::Function {

Object::PyObjPtr Identity(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto obj = args->as<Object::PyList>()->GetItem(0);
  return Object::PyString::Create(
    Collections::CreateIntegerWithU64(reinterpret_cast<uint64_t>(obj.get()))
      .ToHexString()
  );
}

void DebugPrint(const Object::PyObjPtr& obj) {
  ConsoleTerminal::get_instance().info("[DEBUG] ");
  if (obj->is(Object::StringKlass::Self())) {
    ConsoleTerminal::get_instance().info(
      obj->as<Object::PyString>()->ToCppString()
    );
  } else {
    ConsoleTerminal::get_instance().info(
      obj->str()->as<Object::PyString>()->ToCppString()
    );
  }
}

Object::PyObjPtr Print(const Object::PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  auto argList = args->as<Object::PyList>();
  if (argList->Length() == 0) {
    return Object::PyNone::Create();
  }
  std::string result =
    argList->GetItem(0)->str()->as<Object::PyString>()->ToCppString();
  for (Index i = 1; i < argList->Length(); i++) {
    result += " ";
    auto arg = argList->GetItem(i);
    result += arg->str()->as<Object::PyString>()->ToCppString();
  }
  ConsoleTerminal::get_instance().info(result);
  return Object::PyNone::Create();
}

Object::PyObjPtr Len(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<Object::PyList>()->GetItem(0)->len();
}

Object::PyObjPtr Next(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<Object::PyList>()->GetItem(0)->next();
}

Object::PyObjPtr RandInt(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 2);
  auto argList = args->as<Object::PyList>();
  auto left = argList->GetItem(0)->as<Object::PyInteger>();
  auto right = argList->GetItem(1)->as<Object::PyInteger>();
  if (IsTrue(left->ge(
        Object::CreatePyInteger(
          static_cast<uint64_t>(std::numeric_limits<int32_t>::max())
        )
      )) ||
      IsTrue(right->ge(
        Object::CreatePyInteger(
          static_cast<uint64_t>(std::numeric_limits<int32_t>::max())
        )
      )) ||
      IsTrue(left->ge(right))) {
    throw std::runtime_error(
      "RandInt function need left argument less than right argument"
    );
  }
  auto cppLeft = left->ToU64();
  auto cppRight = right->ToU64();
  std::random_device randomDevice;
  std::mt19937 gen(randomDevice());  // 使用 Mersenne Twister 算法生成随机数
  std::uniform_int_distribution<unsigned int>
    dis(  // 生成 [cppLeft, cppRight] 之间的均匀分布
      static_cast<std::mt19937::result_type>(cppLeft),
      static_cast<std::mt19937::result_type>(cppRight)
    );

  auto result = dis(gen);
  return Object::CreatePyInteger(static_cast<int64_t>(result));
}

Object::PyObjPtr Sleep(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto seconds =
    args->as<Object::PyList>()->GetItem(0)->as<Object::PyInteger>();
  if (IsTrue(seconds->lt(Object::CreatePyInteger(0ULL)))) {
    seconds->str()->as<Object::PyString>()->Print();
    throw std::runtime_error("Sleep function need non-negative argument");
  }
  auto secondsValue = seconds->ToU64();
  std::this_thread::sleep_for(std::chrono::seconds(secondsValue));
  return Object::PyNone::Create();
}
Object::PyObjPtr Normal(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 3);
  auto argList = args->as<Object::PyList>();
  auto loc = argList->GetItem(0)->as<Object::PyFloat>()->Value();
  auto scale = argList->GetItem(1)->as<Object::PyFloat>()->Value();
  std::random_device randomDevice;
  std::mt19937 gen(randomDevice());
  std::normal_distribution<> dis(loc, scale);
  auto size = argList->GetItem(2);
  if (size->is(Object::IntegerKlass::Self())) {
    auto sizeValue = size->as<Object::PyInteger>()->ToU64();
    auto result = Object::PyList::Create(Object::PyList::ExpandOnly{sizeValue});
    for (Index i = 0; i < sizeValue; i++) {
      result->SetItem(i, Object::CreatePyFloat(dis(gen)));
    }
    return result;
  }
  if (size->is(Object::ListKlass::Self())) {
    auto sizeList = size->as<Object::PyList>();
    auto row = sizeList->GetItem(0)->as<Object::PyInteger>()->ToU64();
    auto col = sizeList->GetItem(1)->as<Object::PyInteger>()->ToU64();
    auto sizeValue = row * col;
    Collections::List<double> result(sizeValue);
    for (Index i = 0; i < sizeValue; i++) {
      result.Push(dis(gen));
    }
    return Object::PyMatrix::Create(Collections::Matrix(row, col, result));
  }
  throw std::runtime_error("Normal function need integer or list argument");
}

Object::PyObjPtr Shuffle(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto argList = args->as<Object::PyList>();
  auto arg = argList->GetItem(0);
  if (arg->is(Object::ListKlass::Self())) {
    arg->as<Object::PyList>()->Shuffle();
    return Object::PyNone::Create();
  }
  if (arg->is(Object::MatrixKlass::Self())) {
    arg->as<Object::PyMatrix>()->Shuffle();
    return Object::PyNone::Create();
  }
  throw std::runtime_error("Shuffle function need list or matrix argument");
}

Object::PyObjPtr Input(const Object::PyObjPtr& args) {
  CheckNativeFunctionArguments(args);
  if (args->as<Object::PyList>()->Length() > 0) {
    auto prompt = args->as<Object::PyList>()
                    ->GetItem(0)
                    ->as<Object::PyString>()
                    ->ToCppString();
    ConsoleTerminal::get_instance().info(prompt);
  }
  return Object::CreatePyPromise(
    Object::PyNativeFunction::Create([](const Object::PyObjPtr& promiseArgs) {
      // 从Promise参数中获取resolve和reject回调
      auto argsList = promiseArgs->as<Object::PyList>();
      auto resolve = argsList->GetItem(0);
      auto reject = argsList->GetItem(1);

      // 订阅INPUT事件，处理输入内容
      EventBus::get_instance().subscribe(
        EventType::INPUT,
        [resolve, reject](const std::string& input) {
          try {
            // 将输入内容转换为Python字符串对象
            auto pyInput = Object::PyString::Create(input);

            // 调用resolve回调，传递输入内容
            Runtime::Evaluator::InvokeCallable(
              resolve, Object::PyList::Create<Object::PyObjPtr>({pyInput})
            );
          } catch (const std::exception& e) {
            // 处理可能的异常
            Runtime::Evaluator::InvokeCallable(
              reject, Object::PyList::Create<Object::PyObjPtr>(
                        {Object::PyString::Create(e.what())}
                      )
            );
          }

          // 只处理一次输入，完成后取消订阅
          // EventBus::get_instance().unsubscribe(eventId);

          return Object::PyNone::Create();
        }
      );
      return Object::PyNone::Create();
    })
  );
}

auto ReadFile(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto argList = args->as<Object::PyList>();
  auto filePath = argList->GetItem(0)->as<Object::PyString>()->ToCppString();
  return Object::CreatePyPromise(
    Object::PyNativeFunction::Create([filePath](const Object::PyObjPtr& args) {
      auto resolve = args->as<Object::PyList>()->GetItem(0);
      auto reject = args->as<Object::PyList>()->GetItem(1);
      try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
          throw std::runtime_error("File not found: " + filePath);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        auto content = Object::PyString::Create(buffer.str());
        Runtime::Evaluator::InvokeCallable(
          resolve, Object::PyList::Create<Object::PyObjPtr>({content})
        );
      } catch (const std::exception& e) {
        Runtime::Evaluator::InvokeCallable(
          reject, Object::PyList::Create<Object::PyObjPtr>(
                    {Object::PyString::Create(e.what())}
                  )
        );
      }
      return Object::PyNone::Create();
    })
  );
}

// auto Coroutine(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
//   auto argList = args->as<Object::PyList>();
//   auto generator = argList->GetItem(0);
//   auto gen =
//     Runtime::Evaluator::InvokeCallable(generator, Object::PyList::Create({}))
//       ->as<Object::PyGenerator>();
//   auto executor =
//     Object::PyNativeFunction::Create([gen](const Object::PyObjPtr& args) {
//       auto argsList = args->as<Object::PyList>();
//       auto resolve = argsList->GetItem(0);
//       auto reject = argsList->GetItem(1);

//       Object::PyNativeFunctionPtr onFullilled =
//         Object::PyNativeFunction::Create([](const Object::PyObjPtr&) {
//           return Object::PyNone::Create();
//         });

//       onFullilled = Object::PyNativeFunction::Create(
//         [resolve, reject, gen,
//          onFullilled](const Object::PyObjPtr& args) -> Object::PyObjPtr {
//           Object::PyObjPtr ret = Object::PyNone::Create();
//           auto result = args->as<Object::PyList>()->GetItem(0);
//           try {
//             ret = gen->Send(result);
//           } catch (const std::exception& e) {
//             return Runtime::Evaluator::InvokeCallable(
//               reject,
//               Object::PyList::Create({Object::PyString::Create(e.what())})
//             );
//           }
//           if (ret->Klass() == Object::GeneratorKlass::Self()) {
//             ret->as<Object::PyPromise>()->Then(onFullilled);
//           } else {
//             Object::PromiseResolve(Object::PyList::Create({ret}))
//               ->as<Object::PyPromise>()
//               ->Then(onFullilled);
//           }

//           return Object::PyNone::Create();
//         }
//       );
//       Runtime::Evaluator::InvokeCallable(
//         onFullilled, Object::PyList::Create({Object::PyNone::Create()})
//       );
//       return Object::PyNone::Create();
//     });
//   return Object::CreatePyPromise(executor);
// }

Object::PyObjPtr Iter(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  return args->as<Object::PyList>()->GetItem(0)->iter();
}

Object::PyObjPtr Time(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 0);
  auto now = std::chrono::system_clock::now();

  // 分解时间点为秒和纳秒部分
  auto sec = now.time_since_epoch();
  auto sec_count = std::chrono::duration_cast<std::chrono::seconds>(sec);
  auto nanoseconds =
    std::chrono::duration_cast<std::chrono::nanoseconds>(sec - sec_count)
      .count();

  time_t nowTime = std::chrono::system_clock::to_time_t(now);
  tm localTime{};
#if defined(_WIN32) || defined(_WIN64)
  // Windows 平台使用 localtime_s
  localtime_s(&localTime, &nowTime);
#else
  // POSIX 平台使用 localtime_r
  localtime_r(&nowTime, &localTime);
#endif

  std::ostringstream oss;
  oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
  oss << '.' << std::setfill('0') << std::setw(9)
      << nanoseconds;  // 显示纳秒，补零到9位

  ConsoleTerminal::get_instance().info("Current time: " + oss.str());

  return Object::PyNone::Create();
}
/*
 * def range(start, end, step):
if step == 0 or step is None:
    step = 1
if end is None:
    end = start
    start = 0
if step > 0:
    while start < end:
        yield start
        start += step
else:
    while start > end:
        yield start
        start += step
 * */
Object::PyObjPtr Range(const Object::PyObjPtr& args) {
  auto argList = args->as<Object::PyList>();
  int64_t start = 0;
  int64_t end = 0;
  int64_t step = 1;
  if (argList->Length() == 1) {
    end = argList->GetItem(0)->as<Object::PyInteger>()->ToI64();
  } else if (argList->Length() == 2) {
    start = argList->GetItem(0)->as<Object::PyInteger>()->ToI64();
    end = argList->GetItem(1)->as<Object::PyInteger>()->ToI64();
  } else if (argList->Length() == 3) {
    start = argList->GetItem(0)->as<Object::PyInteger>()->ToI64();
    end = argList->GetItem(1)->as<Object::PyInteger>()->ToI64();
    step = argList->GetItem(2)->as<Object::PyInteger>()->ToI64();
  }
  if (step == 0) {
    throw std::runtime_error("Step cannot be zero");
  }
  return Object::CreatePyGenerator(
    [start, end, step](const Object::PyGeneratorPtr& generator) mutable {
      if (step > 0) {
        while (start < end) {
          auto value = Object::CreatePyInteger(start);
          start += step;
          if (start >= end) {
            generator->SetExhausted();
          }
          return value;
        }
      }
      while (start > end) {
        auto value = Object::CreatePyInteger(start);
        start += step;
        if (start <= end) {
          generator->SetExhausted();
        }
        return value;
      }
      throw std::runtime_error("Range generator exhausted");
    }
  );
}

Object::PyObjPtr Type(const Object::PyObjPtr& args) {
  CheckNativeFunctionArgumentsWithExpectedLength(args, 1);
  auto obj = args->getitem(Object::CreatePyInteger(0ULL));
  return obj->Klass()->Type();
}

Object::PyObjPtr BuildClass(const Object::PyObjPtr& args) {
  auto argList = args->as<Object::PyList>();
  // 解析参数：函数、类名和基类
  auto function = argList->GetItem(0)->as<Object::PyFunction>();
  auto name = argList->GetItem(1);
  auto bases = argList->GetItem(2)->as<Object::PyList>();

  // 创建执行环境
  auto globals = function->Globals();
  auto preFrame = Runtime::VirtualMachine::Instance().CurrentFrame();
  auto _name_ = globals->getitem(Object::PyString::Create("__name__"));
  // 保存当前帧
  // 创建新帧并执行类定义函数
  auto frame =
    Object::CreateFrameWithPyFunction(function, Object::PyList::Create());
  auto result = frame->Eval();
  Runtime::VirtualMachine::Instance().BackToParentFrame();
  if (!result->is(Object::NoneKlass::Self())) {
    throw std::runtime_error("Class definition failed");
  }
  // 获取执行结果
  auto classDict = frame->CurrentLocals();
  // 创建新的类型对象
  auto typeName = StringConcat(
                    Object::PyList::Create<Object::PyObjPtr>(
                      {_name_, Object::PyString::Create("."), name}
                    )
  )
                    ->as<Object::PyString>();
  auto* klass = Object::CreatePyKlass(typeName, classDict, bases);
  auto type = Object::CreatePyType(klass);
  return type;
}

auto LogisticLoss(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  Collections::List<double> result(values.Size(), static_cast<double>(0));
  for (Index i = 0; i < values.Size(); i++) {
    double value = values[i];
    if (-value > 1e2) {
      result[i] = -value;
    }
    result[i] = std::log(1 + std::exp(-value));
  }
  return Object::PyMatrix::Create(
    Collections::Matrix(matrix->RowsIndex(), matrix->ColsIndex(), result)
  );
}

auto LogisticLossDerivative(const Object::PyObjPtr& args) noexcept
  -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  Collections::List<double> result(values.Size(), static_cast<double>(0));
  for (Index i = 0; i < values.Size(); i++) {
    double value = values[i];
    if (-value > 1e2) {
      result[i] = -value;
    }
    result[i] = -1 / (1 + std::exp(value));
  }
  return Object::PyMatrix::Create(
    Collections::Matrix(matrix->RowsIndex(), matrix->ColsIndex(), result)
  );
}

auto Sum(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  double result = 0;
  for (Index i = 0; i < values.Size(); i++) {
    result += values[i];
  }
  return Object::CreatePyFloat(result);
}

auto Log(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  Collections::List<double> result(values.Size(), static_cast<double>(0));
  for (Index i = 0; i < values.Size(); i++) {
    double value = values[i];
    result[i] = std::log(value);
  }
  return Object::PyMatrix::Create(
    Collections::Matrix(matrix->RowsIndex(), matrix->ColsIndex(), result)
  );
}

auto SoftMax(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  Collections::List<double> result(values.Size(), static_cast<double>(0));
  double sum = 0;
  for (Index i = 0; i < values.Size(); i++) {
    double value = values[i];
    result[i] = std::exp(value);
    sum += result[i];
  }
  for (Index i = 0; i < values.Size(); i++) {
    result[i] /= sum;
  }
  return Object::PyMatrix::Create(
    Collections::Matrix(matrix->RowsIndex(), matrix->ColsIndex(), result)
  );
}

auto Max(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  double maxValue = values[0];
  for (Index i = 1; i < values.Size(); i++) {
    maxValue = std::max(values[i], maxValue);
  }
  return Object::CreatePyFloat(maxValue);
}

auto ArgMax(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto matrix = argList->GetItem(0)->as<Object::PyMatrix>();
  const Collections::List<double>& values = matrix->Ravel();
  double maxValue = values[0];
  Index maxIndex = 0;
  for (Index i = 1; i < values.Size(); i++) {
    if (values[i] > maxValue) {
      maxValue = values[i];
      maxIndex = i;
    }
  }
  return Object::CreatePyInteger(maxIndex);
}

auto Hash(const Object::PyObjPtr& args) noexcept -> Object::PyObjPtr {
  auto argList = args->as<Object::PyList>();
  auto arg = argList->GetItem(0);
  return arg->hash();
}

}  // namespace kaubo::Function