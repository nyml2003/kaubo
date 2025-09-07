#include "Runtime/EventLoop.h"
#include "Object/Container/PyList.h"
#include "Runtime/VirtualMachine.h"

namespace kaubo::Runtime {

// 假设State枚举定义在头文件中：
// enum class State { Stopped, Running, Waiting };

EventLoop& EventLoop::Instance() {
  static EventLoop instance;
  return instance;
}

void EventLoop::Run() {
  state = State::Running;

  while (true) {
    // 检查是否需要停止

    if (state == State::Stopped) {
      break;
    }

    // 处理所有微任务
    while (!microTaskQueue.empty()) {
      auto task = microTaskQueue.front();
      microTaskQueue.pop();
      Evaluator::InvokeCallable(task, Object::CreatePyList());
    }

    // 处理一个宏任务
    if (!macroTaskQueue.empty()) {
      auto task = macroTaskQueue.front();
      macroTaskQueue.pop();
      Evaluator::InvokeCallable(task, Object::CreatePyList());
      continue;
    }

    if (Idle()) {
      state = State::Stopped;
    };
  }

  state = State::Stopped;
}

void EventLoop::Stop() {
  if (state != State::Stopped) {
    state = State::Stopped;
  }
}

void EventLoop::EnqueueMicroTask(const Object::PyObjPtr& task) {
  bool needRestart = false;
  microTaskQueue.push(task);
  if (state == State::Stopped) {
    needRestart = true;
  }
  if (needRestart) {
    Run();
  }
}

// 宏任务入队也做类似处理
void EventLoop::EnqueueTask(const Object::PyObjPtr& task) {
  bool needRestart = false;

  macroTaskQueue.push(task);

  if (state == State::Stopped) {
    needRestart = true;
  }

  if (needRestart) {
    Run();
  }
}

}  // namespace kaubo::Runtime
