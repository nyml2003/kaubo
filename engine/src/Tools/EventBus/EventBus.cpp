#include "Tools/EventBus/EventBus.h"
namespace kaubo {

EventId EventBus::subscribe(EventType type, const Callback& callback) {
  EventId event_id = generate_id();
  Subscription subscription;
  subscription.id = event_id;
  subscription.callback = callback;
  subscriptions[type].push_back(subscription);
  return event_id;
}

void EventBus::unsubscribe(EventId event_id) {
  for (auto& [type, handlerList] : subscriptions) {
    handlerList.erase(
      std::remove_if(
        handlerList.begin(), handlerList.end(),
        [event_id](const Subscription& subscription) {
          return subscription.id == event_id;
        }
      ),
      handlerList.end()
    );
  }
}

void EventBus::publish(EventType type, const std::string& data) {
  auto& subscriptions_by_type = this->subscriptions[type];
  for (const auto& subscription : subscriptions_by_type) {
    subscription.callback(data);
  }
}

EventId EventBus::generate_id() {
  return ++next_id;
}

}  // namespace kaubo