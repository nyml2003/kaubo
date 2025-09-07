#pragma once
#include "Terminal.h"
#include "TerminalStrategy.h"

namespace kaubo {

class VerboseTerminal : public Terminal, public Singleton<VerboseTerminal> {
  friend class Singleton<VerboseTerminal>;

 public:
  void info(const std::string& msg) override {
    if (m_strategy) {
      // m_strategy->info(std::format("{:{}}{}", "", indent * 2, msg));
      m_strategy->info(msg);
    }
  }

  static void IncreaseIndent() { get_instance().indent++; }
  static void DecreaseIndent() { get_instance().indent--; }

 private:
  VerboseTerminal() = default;
  int indent = 0;
};

}  // namespace kaubo
