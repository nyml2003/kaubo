#pragma once
#include "Collections/Integer/Integer.h"
namespace kaubo::Collections {
// int8_t UnicodeToHex(Unicode unicode) noexcept;
// Unicode HexToUnicode(uint8_t hex) noexcept;
int8_t ByteToHex(Byte byte) noexcept;
Byte HexToByte(uint8_t hex) noexcept;
Integer CreateIntegerWithDecimal(const Decimal& decimal);
void TrimLeadingZero(List<uint32_t>& parts);
void TrimTrailingZero(List<uint32_t>& parts);
Integer Slice(const Integer& integer, Index start, Index end);
Integer CreateIntegerWithString(const String& str);
Integer CreateIntegerWithCString(const char* str);
Integer CreateIntegerZero();
Integer CreateIntegerOne();
Integer CreateIntegerTwo();
Integer CreateIntegerWithU64(uint64_t value, bool sign = false);
Integer CreateIntegerWithI64(int64_t value);
uint64_t ToU64(const Integer& integer);
bool IsBigNumber(const Integer& integer);
int64_t ToI64(const Integer& integer);
}  // namespace kaubo::Collections
