#include "Config.h"
#include "Tools/Json/Lexer/Builder.h"
#include "Tools/Json/Parser/Parser.h"
namespace kaubo {

void Config::init(const std::string& config_json_str) {
  auto lexer = Json::Builder::get_instance();
  lexer->feed(config_json_str);
  lexer->terminate();
  Json::Parser parser(std::move(lexer));
  auto json_result = parser.parse();
  if (json_result.is_err()) {
    throw std::runtime_error("Invalid config file");
  }
  Config::get_instance().config = json_result.unwrap();
}

auto Config::has(const std::string& key) -> bool {
  return Config::get_instance().config->has(key);
}

auto Config::get(const std::string& key) -> std::string {
  return Config::get_instance()
    .config->get(key)
    .unwrap()->get_value<Json::Value::String>()
    ->value;
}

}  // namespace kaubo