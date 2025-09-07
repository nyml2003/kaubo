// NOLINTBEGIN(*)
#pragma once

#include <cstdint>

// 设置导出/导入符号
#define EXPORT_BUILDING_DLL

#if defined(_WIN32)
#if defined(EXPORT_BUILDING_DLL)
#define EXPORT_API __declspec(dllexport)
#elif defined(EXPORT_USING_DLL)
#define EXPORT_API __declspec(dllimport)
#else
#define EXPORT_API
#endif
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

#define EVENT_TYPES(F) \
  F(LOG_INFO, 0)       \
  F(LOG_WARNING, 1)    \
  F(LOG_ERROR, 2)      \
  F(LOG_DEBUG, 3)      \
  F(EXIT_PROGRAM, 4)   \
  F(INPUT, 5)

// -------------------------------
// 生成 C 枚举 EventType
// -------------------------------

typedef enum {
#undef F
#define F(name, val) EVENT_TYPE_##name = val,
  EVENT_TYPES(F)
} C_EventType;

// -------------------------------
// C 接口函数声明
// -------------------------------

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t EventId;
typedef void (*EventCallback)(const char* data);

EXPORT_API EventId eventbus_subscribe(C_EventType type, EventCallback callback);
EXPORT_API void eventbus_unsubscribe(EventId id);
EXPORT_API void eventbus_publish(C_EventType type, const char* data);
EXPORT_API void init_config(const char* config_json_str);
EXPORT_API void compile();
EXPORT_API void interpret();
EXPORT_API void interpret_bytecode();

#ifdef __cplusplus
}
#endif

// -------------------------------
// 生成 C++ 枚举类 EventTypeCpp
// -------------------------------

#ifdef __cplusplus

enum class EventType : uint8_t {
#undef F
#define F(name, val) name = (val),
  EVENT_TYPES(F)
};

// 转换函数 C <-> C++
inline C_EventType to_c(EventType type) {
  return static_cast<C_EventType>(type);
}

inline EventType to_cpp(C_EventType type) {
  return static_cast<EventType>(type);
}

#endif  // __cplusplus

// NOLINTEND(*)