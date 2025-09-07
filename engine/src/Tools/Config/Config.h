#pragma once

#include "Tools/DesignPattern/Singleton.h"
#include "json.hpp"

namespace kaubo {
class Config : public Singleton<Config> {
  friend class Singleton<Config>;

 public:
  static void init(const std::string& config_json_str);

  static bool has(const std::string& key);

  static std::string get(const std::string& key);

 private:
  nlohmann::json config;
};
}  // namespace kaubo