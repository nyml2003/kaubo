#pragma once
#include <queue>
#include "Object/Core/PyObject.h"

namespace kaubo::Runtime {

class EventLoop {
 public:
  // 定义事件循环状态枚举
  enum class State : uint8_t {
    Stopped,  // 已停止：未运行且不会被唤醒
    Running,  // 运行中：正在处理任务
    Waiting   // 等待中：暂时空闲，可被新任务唤醒
  };

  static EventLoop& Instance();
  // 禁止拷贝
  EventLoop(const EventLoop&) = delete;
  EventLoop& operator=(const EventLoop&) = delete;
  // 禁止移动
  EventLoop(EventLoop&&) = delete;
  EventLoop& operator=(EventLoop&&) = delete;

  void Run();
  void Stop();

  void EnqueueTask(const Object::PyObjPtr& task);
  void EnqueueMicroTask(const Object::PyObjPtr& task);
  [[nodiscard]] bool Idle() const {
    return macroTaskQueue.empty() && microTaskQueue.empty();
  }

  // 获取当前状态
  [[nodiscard]] State GetState() const { return state; }

 private:
  std::queue<Object::PyObjPtr> macroTaskQueue;  // 宏任务队列
  std::queue<Object::PyObjPtr> microTaskQueue;  // 微任务队列
  State state = State::Stopped;                 // 初始状态为停止

  // 单例模式私有构造函数
  EventLoop() = default;
  ~EventLoop() { Stop(); }
};

}  // namespace kaubo::Runtime


