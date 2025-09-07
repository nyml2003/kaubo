#pragma once

#include "Collections/String/String.h"

#include <string>

namespace kaubo::Collections {
String CreateStringWithCString(const char* str) noexcept;
template <typename T>
String ToString(T value) {
  return CreateStringWithCString(std::to_string(value).c_str());
}
std::size_t Hash(const List<Byte>& str) noexcept;
}  // namespace kaubo::Collections
