#pragma once

#include "Binding/C_API/c_api.h"
#include "Tools/DesignPattern/Singleton.h"

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace kaubo {
using Callback = std::function<void(std::string)>;

using EventId = uint32_t;

struct Subscription {
  EventId id;
  Callback callback;
};

class EventBus : public Singleton<EventBus> {
  friend class Singleton<EventBus>;

 public:
  // 订阅某个事件类型，返回唯一 EventId
  EventId subscribe(EventType type, const Callback& callback);

  // 根据 EventId 取消订阅
  void unsubscribe(EventId event_id);

  // 发布事件到指定类型
  void publish(EventType type, const std::string& data);

 private:
  EventId generate_id();

  std::unordered_map<EventType, std::vector<Subscription>> subscriptions;
  std::mutex mtx;
  EventId next_id = 0;
};
}  // namespace kaubo
