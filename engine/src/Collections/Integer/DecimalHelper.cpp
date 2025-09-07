#include "Collections/Integer/DecimalHelper.h"
#include <stdexcept>
#include "Collections/String/StringHelper.h"
#include "Common.h"

namespace kaubo::Collections {
Decimal CreateDecimalZero() {
  return CreateDecimalWithU32(0);
}
Decimal CreateDecimalOne() {
  return CreateDecimalWithU32(1);
}
Decimal CreateDecimalWithU32(uint32_t value) {
  List<int32_t> parts;
  while (value > 0) {
    parts.Push(static_cast<int32_t>(value % Decimal::radix));
    value /= Decimal::radix;
  }
  parts.Reverse();
  return Decimal(parts, false);
}
Byte DecToByte(int32_t dec) noexcept {
  return static_cast<Byte>(static_cast<Byte>(dec) + Byte_0);
}
int32_t ByteToDec(Byte byte) noexcept {
  if (byte >= Byte_0 && byte <= Byte_9) {
    return static_cast<int32_t>(byte - Byte_0);
  }
  return -1;
}
Decimal CreateDecimalWithString(const String& str) {
  Index index = 0;
  bool sign = false;
  if (str.GetCodeUnit(0) == '-') {
    sign = true;
    index = 1;
  }
  List<int32_t> parts;
  for (; index < str.GetCodeUnitCount(); ++index) {
    int32_t character = ByteToDec(str.GetCodeUnit(index));
    if (character == -1) {
      throw std::runtime_error("Invalid character in Decimal");
    }
    parts.Push(character);
  }
  return Decimal(parts, sign);
}
void TrimTrailingZero(List<int32_t>& parts) {
  for (Index i = parts.Size() - 1; (~i) != 0U; --i) {
    if (parts[i] != 0 || i == 0) {
      parts = parts.Slice(0, i + 1);
      break;
    }
  }
}
void TrimLeadingZero(List<int32_t>& parts) {
  for (Index i = 0; i < parts.Size(); i++) {
    if (parts[i] != 0 || i == parts.Size() - 1) {
      parts = parts.Slice(i, parts.Size());
      break;
    }
  }
}
Decimal CreateDecimalWithCString(const char* str) {
  return CreateDecimalWithString(CreateStringWithCString(str));
}
}  // namespace kaubo::Collections