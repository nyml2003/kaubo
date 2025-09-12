#pragma once

#include <memory>

namespace kaubo::Object {
template <typename T>
class IObjectCreator {
 public:
  template <typename... Args>
  static std::shared_ptr<T> create(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  virtual ~IObjectCreator() = default;
  explicit IObjectCreator() = default;
  IObjectCreator(const IObjectCreator&) = delete;
  IObjectCreator& operator=(const IObjectCreator&) = delete;
  IObjectCreator(IObjectCreator&&) = delete;
  IObjectCreator& operator=(IObjectCreator&&) = delete;
};
}  // namespace kaubo::Object