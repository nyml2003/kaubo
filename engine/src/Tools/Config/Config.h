#pragma once

#include "Tools/DesignPattern/Singleton.h"

#include "Tools/Json/Parser/Value.h"

namespace kaubo {
class Config : public Singleton<Config> {
  friend class Singleton<Config>;

 public:
  static void init(const std::string& config_json_str);

  static auto has(const std::string& key) -> bool;

  static auto get(const std::string& key) -> std::string;

 private:
  Json::Value::ValuePtr config;
};
}  // namespace kaubo