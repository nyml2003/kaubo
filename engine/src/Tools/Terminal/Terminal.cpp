
#include "Terminal.h"
#include "TerminalStrategy.h"

namespace kaubo {

Terminal::Terminal()
  : m_strategy(
      std::make_unique<ProxyTerminalStrategy>(&ConsoleTerminal::get_instance())
    ) {}

}  // namespace kaubo