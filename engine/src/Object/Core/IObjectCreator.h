#pragma once

#include <memory>

namespace kaubo::Object {
template <typename T>
class IObjectCreator {
 public:
  template <typename... Args>
  static auto Create(Args&&... args) -> std::shared_ptr<T> {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template <typename U>
  static auto Create(std::initializer_list<U> list) -> std::shared_ptr<T> {
    return std::make_shared<T>(list);  // 转发给 T 的构造函数
  }

  virtual ~IObjectCreator() = default;
  explicit IObjectCreator() = default;
  IObjectCreator(const IObjectCreator&) = delete;
  IObjectCreator& operator=(const IObjectCreator&) = delete;
  IObjectCreator(IObjectCreator&&) = delete;
  IObjectCreator& operator=(IObjectCreator&&) = delete;
};
}  // namespace kaubo::Object