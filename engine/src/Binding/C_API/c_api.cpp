#include "c_api.h"
#include "Binding/Interface/Interface.h"
#include "Tools/Config/Config.h"
#include "Tools/EventBus/EventBus.h"

extern "C" {

EventId eventbus_subscribe(C_EventType type, EventCallback callback) {
  if (callback == nullptr) {
    return 0;
  }

  return kaubo::EventBus::get_instance().subscribe(
    to_cpp(type),
    [callback](const std::string& data) { callback(data.c_str()); }
  );
}

void eventbus_unsubscribe(EventId event_id) {
  if (event_id == 0U) {
    return;
  }
  kaubo::EventBus::get_instance().unsubscribe(event_id);
}

void eventbus_publish(C_EventType type, const char* data) {
  if (data == nullptr) {
    return;
  }
  kaubo::EventBus::get_instance().publish(to_cpp(type), std::string(data));
}

void init_config(const char* config_json_str) {
  kaubo::Config::init(config_json_str);
}

void compile() {
  kaubo::force_utf8();
  kaubo::init_runtime_support();
  kaubo::handle_mode_compile();
}

void interpret() {
  kaubo::force_utf8();
  kaubo::init_runtime_support();
  kaubo::handle_mode_interpret();
}

void interpret_bytecode() {
  kaubo::force_utf8();
  kaubo::init_runtime_support();
  kaubo::handle_mode_interpret_bytecode();
}

}  // extern "C"