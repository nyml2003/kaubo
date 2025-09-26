#pragma once

#include "Utils/Result.h"

#include <map>
#include <memory>
#include <vector>

namespace kaubo::Json::Value {
using kaubo::Utils::Err;
using kaubo::Utils::Ok;
using kaubo::Utils::Result;
struct Null {};
struct True {};
struct False {};
struct Number {
  int64_t value;
};
struct String {
  std::string value;
};

class Value;
using ValuePtr = std::shared_ptr<Value>;
struct Array {
  std::vector<ValuePtr> value;
};
struct Object {
  std::map<std::string, ValuePtr> value;
};

using ValueType = std::variant<
  std::shared_ptr<Null>,
  std::shared_ptr<True>,
  std::shared_ptr<False>,
  std::shared_ptr<Number>,
  std::shared_ptr<String>,
  std::shared_ptr<Array>,
  std::shared_ptr<Object>>;

class Value {
 public:
  explicit Value() { m_value = std::make_shared<Object>(); }

  auto get(const std::string& key) -> Result<ValuePtr, std::string>;
  auto set(const std::string& key, ValuePtr value)
    -> Result<ValuePtr, std::string>;

  auto has(const std::string& key) -> bool;

  template <typename T>
  explicit Value(std::shared_ptr<T> value) : m_value(std::move(value)) {}

  [[nodiscard]] auto to_string() const -> std::string;

  template <typename T>
  [[nodiscard]] auto get_value() const -> std::shared_ptr<T> {
    return std::get<std::shared_ptr<T>>(m_value);
  }

 private:
  ValueType m_value;
};

}  // namespace kaubo::Json::Value