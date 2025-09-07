#pragma once
#include <cstdint>
namespace kaubo {
using Byte = uint8_t;                     // 将无符号8位整数定义为Byte
using Unicode = uint32_t;                 // 将无符号32位整数定义为Unicode
using Index = uint64_t;                   // 所有索引默认为无符号64位整数
struct None {};                           // None 类型是一个空结构体
const Byte Byte_0 = '0';                  // Byte_0 是字符 '0'
const Byte Byte_9 = '9';                  // Byte_9 是字符 '9'
const Byte Byte_A = 'A';                  // Byte_A 是字符 'A'
const Byte Byte_F = 'F';                  // Byte_F 是字符 'F'
const Byte Byte_a = 'a';                  // Byte_a 是字符 'a'
const Byte Byte_f = 'f';                  // Byte_f 是字符 'f'
const Byte Byte_X = 'X';                  // Byte_X 是字符 'X'
const Byte Byte_x = 'x';                  // Byte_x 是字符 'x'
const Byte Byte_Upper_Lower_Diff = 0x20;  // 大写字母和小写字母的差值
const Byte ByteMinus = '-';               // Byte_0x2D 是字符 '-'
const Byte BytePlus = '+';                // Byte_0x2B 是字符 '+'
const int8_t HexOffset = 10;              // HexOffset 是16进制字母的偏移量
const int8_t MaxHexValue = 16;            // MaxHexValue 是16进制的最大值
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;
}  // namespace kaubo
