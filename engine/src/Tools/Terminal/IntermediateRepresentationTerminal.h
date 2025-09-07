#pragma once

#include "Terminal.h"
#include "Tools/DesignPattern/Singleton.h"

namespace kaubo {

class IntermediateRepresentationTerminal
  : public Terminal,
    public Singleton<IntermediateRepresentationTerminal> {
 public:
  void info(const std::string& msg) override {
    // 第一次调用时输出 mermaid 图表开头
    if (!m_initialized) {
      m_strategy->info("```mermaid");
      m_strategy->info("graph LR");
      m_initialized = true;
    }
    m_strategy->info(msg);
  }

  void terminate() {
    if (m_initialized) {
      m_strategy->info("```");  // 结束 mermaid 图表
      m_initialized = false;    // 标记为未初始化，以便下次重新开始新的图表
    }
  }

 private:
  bool m_initialized{};
};

}  // namespace kaubo