
#include "Collections/String/StringHelper.h"
#include "Collections/String/String.h"

namespace kaubo::Collections {
String CreateStringWithCString(const char* str) noexcept {
  constexpr std::size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
  constexpr std::size_t FNV_PRIME = 1099511628211ULL;

  std::size_t hash = FNV_OFFSET_BASIS;
  Index length = 0;
  for (; str[length] != '\0'; ++length) {
    hash ^= static_cast<std::size_t>(str[length]);
    hash *= FNV_PRIME;
  }
  return String(List<Byte>(length, reinterpret_cast<const Byte*>(str)), hash);
}
std::size_t Hash(const List<Byte>& str) noexcept {
  // FNV-1a 常量（64 位版本）
  constexpr std::size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
  constexpr std::size_t FNV_PRIME = 1099511628211ULL;

  std::size_t hash = FNV_OFFSET_BASIS;

  for (Index i = 0; i < str.Size(); ++i) {
    hash ^= static_cast<std::size_t>(str[i]);
    hash *= FNV_PRIME;
  }
  return hash;
}

}  // namespace kaubo::Collections