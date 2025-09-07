#pragma once
#include "Collections/Integer/Decimal.h"
#include "Collections/Integer/Integer.h"
#include "Collections/String/String.h"
namespace kaubo::Collections {

void Write(const String& bytes, const std::string& filename);
std::string ToCppString(const String& bytes);
String ReprByte(Byte byte);
String Serialize(double value);
String Serialize(uint64_t value);
uint64_t DeserializeU64(const List<Byte>& bytes);
uint64_t DeserializeU64(const List<Byte>& bytes, Index& offset);
String Serialize(int64_t value);
int64_t DeserializeI64(const List<Byte>& bytes, Index& offset);
String Serialize(uint32_t value);
String Serialize(int32_t value);
String Serialize(uint16_t value);
uint16_t DeserializeU16(const List<Byte>& bytes);
String Serialize(const Integer& value);
Integer DeserializeInteger(const List<Byte>& bytes);
String Serialize(const Decimal& value);
String Serialize(const String& value);
}  // namespace kaubo::Collections
