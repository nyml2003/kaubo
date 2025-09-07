#pragma once

namespace kaubo {
template <typename T>
class Singleton {
 public:
  // 获取单例实例
  static T& get_instance() {
    static T instance;
    return instance;
  }

  // 删除拷贝构造和赋值操作符
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

  // 删除移动构造和赋值操作符
  Singleton(Singleton&&) = delete;
  Singleton& operator=(Singleton&&) = delete;

 protected:
  // 保护构造函数和析构函数，防止外部直接构造
  Singleton() = default;
  virtual ~Singleton() = default;
};
}  // namespace kaubo