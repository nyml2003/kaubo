#include "Collections/Integer/IntegerHelper.h"
#include "Collections/Integer/Decimal.h"
#include "Collections/Integer/DecimalHelper.h"
#include "Collections/Integer/Integer.h"
#include "Collections/String/StringHelper.h"
#include "Common.h"

#include <stdexcept>
namespace kaubo::Collections {
int8_t ByteToHex(Byte byte) noexcept {
  if (byte >= Byte_0 && byte <= Byte_9) {
    return static_cast<int8_t>(byte - Byte_0);
  }
  if (byte >= Byte_A && byte <= Byte_F) {
    return static_cast<int8_t>(byte - Byte_A + HexOffset);
  }
  if (byte >= Byte_a && byte <= Byte_f) {
    return static_cast<int8_t>(byte - Byte_a + HexOffset);
  }
  return -1;
}
// Unicode HexToUnicode(uint8_t hex) noexcept {
//   if (hex <= 9) {
//     return static_cast<Unicode>(hex + Unicode_0);
//   }
//   if (hex >= 10 && hex <= 15) {
//     return static_cast<Unicode>(hex - 10 + Unicode_A);
//   }
//   return 0;
// }
Byte HexToByte(uint8_t hex) noexcept {
  if (hex < HexOffset) {
    return static_cast<Byte>(hex + Byte_0);
  }
  if (hex >= HexOffset && hex < MaxHexValue) {
    return static_cast<Byte>(hex - HexOffset + Byte_A);
  }
  return 0;
}

Integer CreateIntegerWithString(const String& str) {
  if (str.GetCodeUnitCount() > 2 && str.GetCodeUnit(0) == Byte_0 &&
      (str.GetCodeUnit(1) == Byte_x || str.GetCodeUnit(1) == Byte_X)) {
    List<uint32_t> parts;
    uint32_t buffer = 0;
    uint32_t count = 0;
    for (Index i = str.GetCodeUnitCount() - 1; i >= 2; i--) {
      int8_t value = ByteToHex(str.GetCodeUnit(i));
      if (value == -1) {
        throw std::runtime_error("Invalid character in Hexadecimal");
      }
      buffer = (static_cast<uint32_t>(value) << (count * 4)) | buffer;
      count++;
      if (count == 4) {
        parts.Push(buffer);
        buffer = 0;
        count = 0;
      }
    }
    if (count != 0) {
      parts.Push(buffer);
    }
    parts.Reverse();
    return Integer(parts, false);
  }
  return CreateIntegerWithDecimal(CreateDecimalWithString(str));
}
Integer CreateIntegerWithCString(const char* str) {
  return CreateIntegerWithString(CreateStringWithCString(str));
}
Decimal CreateDecimalWithInteger(const Integer& integer) {
  Decimal decimal = CreateDecimalZero();
  const auto& parts = integer.Data();
  for (Index i = 0; i < parts.Size(); i++) {
    Decimal temp = CreateDecimalWithU32(parts.Get(i));
    decimal = decimal.Multiply(CreateDecimalWithU32(MaxDecimalPart)).Add(temp);
  }
  return Decimal(decimal.Data(), integer.Sign());
}
Integer CreateIntegerWithDecimal(const Decimal& decimal) {
  List<uint32_t> parts;
  Decimal temp = decimal.Copy();
  Decimal num65536(CreateDecimalWithString(CreateStringWithCString("65536")));
  while (!temp.IsZero()) {
    List<Decimal> divmod = temp.DivMod(num65536);
    temp = divmod.Get(0);
    Decimal remainder = divmod.Get(1);
    int32_t value = 0;
    for (Index i = 0; i < remainder.Data().Size(); i++) {
      value = value * Decimal::radix + remainder.Data().Get(i);
    }
    parts.Push(static_cast<uint32_t>(value));
  }
  parts.Reverse();
  Integer result(parts, decimal.Sign());
  return result;
}
void TrimTrailingZero(List<uint32_t>& parts) {
  for (Index i = parts.Size() - 1; (~i) != 0U; i--) {
    if (parts.Get(i) != 0 || i == 0) {
      parts = parts.Slice(0, i + 1);
      break;
    }
  }
}
void TrimLeadingZero(List<uint32_t>& parts) {
  for (Index i = 0; i < parts.Size(); i++) {
    if (parts.Get(i) != 0 || i == parts.Size() - 1) {
      parts = parts.Slice(i, parts.Size());
      break;
    }
  }
}
Integer Slice(const Integer& integer, Index start, Index end) {
  List<uint32_t> parts = integer.Data().Slice(start, end);
  return Integer(parts, integer.Sign());
}
Integer CreateIntegerZero() {
  return Integer(List<uint32_t>({0}), false);
}
Integer CreateIntegerOne() {
  return Integer(List<uint32_t>({1}), false);
}
Integer CreateIntegerTwo() {
  return Integer(List<uint32_t>({2}), false);
}
uint64_t ToU64(const Integer& integer) {
  if (integer.IsZero()) {
    return 0;
  }
  if (integer.Sign()) {
    throw std::runtime_error("Negative integer cannot be converted to index");
  }
  auto data = integer.Data();
  if (data.Size() > 4) {
    throw std::runtime_error("Integer is too large to be converted to index");
  }
  Index result = 0;
  for (Index i = 0; i < data.Size(); i++) {
    result = (result << Integer::radix) | data.Get(i);
  }
  return result;
}
Integer CreateIntegerWithU64(uint64_t value, bool sign) {
  List<uint32_t> parts;
  while (value != 0) {
    parts.Push(value & Integer::low16Mask);
    value >>= Integer::radix;
  }
  parts.Reverse();
  return Integer(parts, sign);
}

bool IsBigNumber(const Integer& integer) {
  return integer.Data().Size() > 4;
}

Integer CreateIntegerWithI64(int64_t value) {
  if (value == 0) {
    return CreateIntegerZero();
  }
  if (value < 0) {
    return CreateIntegerWithU64(static_cast<uint64_t>(-value), true);
  }
  return CreateIntegerWithU64(static_cast<uint64_t>(value), false);
}
int64_t ToI64(const Integer& integer) {
  if (integer.IsZero()) {
    return 0;
  }
  if (integer.Data().Size() > 4) {
    throw std::runtime_error("Integer is too large to be converted to index");
  }
  int64_t result = 0;
  for (Index i = 0; i < integer.Data().Size(); i++) {
    result = (result << Integer::radix) | integer.Data().Get(i);
  }
  return integer.Sign() ? -result : result;
}

}  // namespace kaubo::Collections
