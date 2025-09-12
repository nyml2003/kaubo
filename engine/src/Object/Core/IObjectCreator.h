#pragma once

#include <memory>

namespace kaubo::Object {
template <typename T>
class IObjectCreator {
 public:
  template <typename... Args>
  static std::shared_ptr<T> Create(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template <typename U>
  static std::shared_ptr<T> Create(std::initializer_list<U> list) {
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