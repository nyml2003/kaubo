#pragma once

#include <functional>
#include "Object/Container/PyList.h"

#include <string>
namespace kaubo::Object {
using TypeFunction = std::function<PyObjPtr(PyObjPtr)>;
// 外部定义的辅助函数模板（可放在头文件或命名空间中）
// 外部辅助函数：处理索引序列并赋值到 params
template <typename Tuple, typename ArgList, size_t... I>
void assign_indices(
  Tuple& params,
  const ArgList& arg_list,
  std::index_sequence<I...> /*unused*/
) {
  (void(std::get<I>(params) = arg_list.GetItem(I)), ...);
}

// 外部辅助函数：展开索引序列
template <typename F, typename... Args, size_t... I>
void expand_indices(
  std::index_sequence<I...> /*unused*/,
  F&& func,
  Args&&... /*unused*/
) {
  std::forward<F>(func)(std::integral_constant<size_t, I>{}...);
}

template <typename T, typename... Args>
PyObjPtr ForwardFunction(const PyObjPtr& args, PyObjPtr (T::*func)(Args...)) {
  static_assert(
    std::is_base_of_v<Klass, T>, "T must core_class_inherit from Klass"
  );

  if (!args->is(ListKlass::Self())) {
    throw std::runtime_error("ForwardFunction expects a list as input");
  }
  auto argList = args->as<PyList>();
  if (argList->Length() != sizeof...(Args)) {
    throw std::runtime_error(
      "ForwardFunction expects a list with " + std::to_string(sizeof...(Args)) +
      " elements"
    );
  }

  std::tuple<std::decay_t<Args>...> params;

  // 使用外部的 assign_indices 和索引序列
  assign_indices(
    params,
    *argList,  // 假设 argList 是 shared_ptr 或指针，需要解引用
    std::make_index_sequence<sizeof...(Args)>{}
  );

  return std::apply(
    [&](auto&&... args) {
      return (static_cast<T*>(T::Self())->*func)(std::move(args)...);
    },
    params
  );
}

template <typename T, typename... Args>
TypeFunction CreateForwardFunction(PyObjPtr (T::*func)(Args...)) {
  return [func](const PyObjPtr& args) -> PyObjPtr {
    return ForwardFunction<T>(args, func);
  };
}
}  // namespace kaubo::Object
