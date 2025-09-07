#include "Terminal.h"
#include "TerminalStrategy.h"

namespace kaubo {
void ProxyTerminalStrategy::info(const std::string& msg) const {
  terminal->info(msg);
}
void ProxyTerminalStrategy::error(const std::string& msg) const {
  terminal->error(msg);
}
void ProxyTerminalStrategy::warn(const std::string& msg) const {
  terminal->warn(msg);
}
void ProxyTerminalStrategy::debug(const std::string& msg) const {
  terminal->debug(msg);
}
}  // namespace kaubo