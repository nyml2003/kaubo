#include "Terminal.h"
#include "TerminalStrategy.h"

namespace kaubo {
ConsoleTerminal::ConsoleTerminal()
  : Terminal(std::make_unique<StardardTerminalStrategy>()) {}
}  // namespace kaubo