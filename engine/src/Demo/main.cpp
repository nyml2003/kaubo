#include <iostream>
#include <string>
#include <thread>
#include "Binding/C_API/c_api.h"
#include "json.hpp"

// 使用命名空间简化代码
using json = nlohmann::json;
namespace {
/**
 * @brief 持续读取标准输入并发布到事件总线
 * @param stop_flag 用于通知线程停止的标志
 */
void forward_stdin_to_eventbus(std::atomic<bool>& stop_flag) {
  try {
    std::string input;
    // 持续读取直到EOF、程序结束或收到停止信号
    while (!stop_flag && std::getline(std::cin, input)) {
      // 将读取到的内容发布到事件总线
      eventbus_publish(EVENT_TYPE_INPUT, input.c_str());
    }
  } catch (const std::exception& e) {
    std::cerr << "Error in input forwarding thread: " << e.what() << '\n';
  }
}
}  // namespace

int main() {
  try {
    // 配置初始化
    json config;
    config["file"] = R"(C:\Users\nyml\code\kaubo\engine\test\dev\dev.kaubo)";
    config["interpret"] = true;

    // 初始化配置并检查结果
    init_config(config.dump().c_str());

    // 订阅日志信息事件
    eventbus_subscribe(EVENT_TYPE_LOG_INFO, [](const char* msg) {
      if (msg) {
        std::cout << msg << '\n';
      }
    });

    // 线程控制标志
    std::atomic<bool> stop_flag(false);

    // 启动输入转发线程
    std::thread input_thread(forward_stdin_to_eventbus, std::ref(stop_flag));

    // 执行解释器
    interpret();

    // 通知线程停止并等待其结束
    stop_flag = true;
    if (input_thread.joinable()) {
      input_thread.join();
    }

    // 清理资源（如果有对应的API）
    // cleanup_config();

  } catch (const std::exception& e) {
    std::cerr << "Program error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
