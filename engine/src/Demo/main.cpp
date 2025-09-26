#include <iostream>
#include <string>
#include <thread>
#include "Binding/C_API/c_api.h"

#include "Tools/Json/Parser/Utils.h"
#include "Tools/Json/Parser/Value.h"

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
    kaubo::Json::Value::ValuePtr config =
      kaubo::Json::Utils::create<kaubo::Json::Value::Value>(
        kaubo::Json::Utils::create<kaubo::Json::Value::Object>({})
      );
    config->set(
      std::string("file"),
      kaubo::Json::Utils::create<kaubo::Json::Value::Value>(
        kaubo::Json::Utils::create<kaubo::Json::Value::String>(
          std::string(R"(C:\Users\nyml\code\kaubo\engine\test\dev\dev.kaubo)")
        )
      )
    );
    config->set(
      "interpret", kaubo::Json::Utils::create<kaubo::Json::Value::Value>(
                     kaubo::Json::Utils::create<kaubo::Json::Value::True>()
                   )
    );
    // 初始化配置并检查结果
    init_config(config->to_string().c_str());

    // 订阅日志信息事件
    eventbus_subscribe(EVENT_TYPE_LOG_INFO, [](const char* msg) {
      if (msg) {
        std::cout << msg << '\n';
      }
    });

    eventbus_subscribe(EVENT_TYPE_LOG_DEBUG, [](const char* msg) {
      if (msg) {
        std::cout << msg << '\n';
      }
    });

    // 执行解释器
    interpret();

    // 清理资源（如果有对应的API）
    // cleanup_config();

  } catch (const std::exception& e) {
    std::cerr << "Program error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
