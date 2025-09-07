#pragma once

#include "Tools/EventBus/EventBus.h"

#include <fstream>
#include <iostream>
#include <string>

namespace kaubo {

class Terminal;
class TerminalStrategy {
 public:
  virtual ~TerminalStrategy() = default;
  virtual void info(const std::string& msg) const = 0;
  virtual void warn(const std::string& msg) const = 0;
  virtual void error(const std::string& msg) const = 0;
  virtual void debug(const std::string& msg) const = 0;

  TerminalStrategy() = default;

  TerminalStrategy(const TerminalStrategy&) = delete;
  TerminalStrategy& operator=(const TerminalStrategy&) = delete;
  TerminalStrategy(TerminalStrategy&&) = delete;
  TerminalStrategy& operator=(TerminalStrategy&&) = delete;
};

class StardardTerminalStrategy : public TerminalStrategy {
 public:
  void info(const std::string& msg) const override {
    EventBus::get_instance().publish(EventType::LOG_INFO, msg);
  }
  void warn(const std::string& msg) const override {
    EventBus::get_instance().publish(EventType::LOG_WARNING, msg);
  }
  void error(const std::string& msg) const override {
    EventBus::get_instance().publish(EventType::LOG_ERROR, msg);
  }
  void debug(const std::string& msg) const override {
    EventBus::get_instance().publish(EventType::LOG_DEBUG, msg);
  }
};

class FileTerminalStrategy : public TerminalStrategy {
 public:
  explicit FileTerminalStrategy(const std::string& filename);

  void info(const std::string& msg) const override { m_stream << msg << '\n'; }
  void warn(const std::string& msg) const override { m_stream << msg << '\n'; }
  void error(const std::string& msg) const override { m_stream << msg << '\n'; }
  void debug(const std::string& msg) const override { m_stream << msg << '\n'; }

 private:
  mutable std::ofstream m_stream;
};

class ProxyTerminalStrategy : public TerminalStrategy {
 public:
  explicit ProxyTerminalStrategy(Terminal* terminal) : terminal(terminal) {}

  void info(const std::string& msg) const override;
  void warn(const std::string& msg) const override;
  void error(const std::string& msg) const override;
  void debug(const std::string& msg) const override;

 private:
  // Terminal使用单例模式，因此使用指针
  Terminal* terminal;
};

}  // namespace kaubo
