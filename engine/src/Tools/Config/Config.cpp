#include "Config.h"

namespace kaubo {

void Config::init(const std::string& config_json_str) {
  Config::get_instance().config = nlohmann::json::parse(config_json_str);
}

bool Config::has(const std::string& key) {
  return Config::get_instance().config.contains(key);
}

std::string Config::get(const std::string& key) {
  return Config::get_instance().config[key];
}

}  // namespace kaubo