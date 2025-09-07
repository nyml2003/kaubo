#pragma once

#include "TerminalStrategy.h"
#include "Tools/DesignPattern/Singleton.h"

namespace kaubo {
class Terminal {
 public:
  virtual ~Terminal() = default;

  void switch_strategy(std::unique_ptr<TerminalStrategy> strategy) {
    m_strategy = std::move(strategy);
  }

  // 每个 Terminal 都要实现自己的 info、warn、error、debug 方法
  virtual void info(const std::string& msg) { m_strategy->info(msg); }
  virtual void warn(const std::string& msg) { m_strategy->warn(msg); }
  virtual void error(const std::string& msg) { m_strategy->error(msg); }
  virtual void debug(const std::string& msg) { m_strategy->debug(msg); }
  Terminal(const Terminal&) = delete;
  Terminal& operator=(const Terminal&) = delete;
  Terminal(Terminal&&) = delete;
  Terminal& operator=(Terminal&&) = delete;

 protected:
  explicit Terminal();
  explicit Terminal(std::unique_ptr<TerminalStrategy> strategy)
    : m_strategy(std::move(strategy)) {}
  // NOLINTNEXTLINE
  std::unique_ptr<TerminalStrategy> m_strategy;
};

class BytecodeTerminal : public Terminal, public Singleton<BytecodeTerminal> {
  friend class Singleton<BytecodeTerminal>;
};

class ConsoleTerminal : public Terminal, public Singleton<ConsoleTerminal> {
  friend class Singleton<ConsoleTerminal>;

 public:
  ConsoleTerminal();
};

class LexicalAnalysisTerminal : public Terminal,
                                public Singleton<LexicalAnalysisTerminal> {
  friend class Singleton<LexicalAnalysisTerminal>;
};
class SyntaxAnalysisTerminal : public Terminal,
                               public Singleton<SyntaxAnalysisTerminal> {
  friend class Singleton<SyntaxAnalysisTerminal>;
};
class ResultTerminal : public Terminal, public Singleton<ResultTerminal> {
  friend class Singleton<ResultTerminal>;
};

}  // namespace kaubo
