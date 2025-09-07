#pragma once

#include <array>
#include <cstdint>
#include "Collections/List.h"
namespace kaubo::Collections {

class String {
  friend class StringBuilder;

 private:
  /// @brief 码元（Code Units）是UTF-8编码中的基本单位。
  /// @details
  /// 对于UTF-8编码，每个码元是8位（1字节）。一个码点可能由1到4个码元组成，
  ///          具体取决于该字符的Unicode范围。
  List<Byte> codeUnits;

  /// @brief 码点列表，表示每个码点。
  /// @details
  /// 采用懒计算策略：仅在需要时计算部分码点的索引，而不是一次性计算所有索引。
  ///          例如，如果要访问第60个码点，则只计算前60个码点。
  mutable List<Unicode> codePoints;

  static constexpr std::array<uint8_t, 256> utf8_sequence_length =
    []() constexpr {
      std::array<uint8_t, 256> table = {};  // 默认值为 0

      // ASCII 字符 (0x00 - 0x7F) → 1 字节
      for (uint8_t i = 0x00; i <= 0x7F; ++i) {
        table[i] = 1;
      }

      // 2 字节序列 (0xC0 - 0xDF)
      for (uint8_t i = 0xC0; i <= 0xDF; ++i) {
        table[i] = 2;
      }

      // 3 字节序列 (0xE0 - 0xEF)
      for (uint8_t i = 0xE0; i <= 0xEF; ++i) {
        table[i] = 3;
      }

      // 4 字节序列 (0xF0 - 0xF7)
      for (uint8_t i = 0xF0; i <= 0xF7; ++i) {
        table[i] = 4;
      }

      return table;
    }();

  /// @brief 码点索引列表，表示每个码点在码元中的起始位置。
  /// @details
  /// 每个Unicode对象的起始位置在codeUnits中的索引。
  /// 字符串的本质是码元的集合，而不是码点的集合。
  mutable List<Index> codePointIndices;

  /// @brief 当前下一个需要解析的码点偏移。
  /// @details 表示在codeUnits中尚未解析的下一个码点的起始位置。
  ///          初始值为0，表示从头开始解析。每次解析新的码点时，从该位置开始读取，
  ///          并更新此值以指向下一个未解析的码点。
  mutable Index unparsedCodeUnitOffset = 0;

  /// @brief 码点是否全部解析完成。
  bool HasParsedAllCodePoints() const {
    return unparsedCodeUnitOffset >= codeUnits.Size();
  }

  /// @brief 解析下一个码点。
  /// @details
  /// 解析当前码元的Unicode值，并更新unparsedCodeUnitOffset，codePoints和codePointIndices。
  void ParseCodePoint() const noexcept;

  /// @brief 解析指定索引处的码点。
  /// @details
  /// 解析指定索引处的Unicode值，并更新unparsedCodeUnitOffset，codePoints和codePointIndices。
  void ParseCodePointAt(Index index);

  /// @brief 完全解析当前码点。
  /// @details
  /// 解析当前码点的Unicode值，并更新unparsedCodeUnitOffset，codePoints和codePointIndices。
  /// 用于在需要遍历所有码点时使用。
  void ParseCodePoints() const noexcept {
    while (!HasParsedAllCodePoints()) {
      ParseCodePoint();
    }
  }

  /// @brief 哈希值，用于快速比较字符串内容。
  /// @details 可以通过计算整个字符串的哈希值来优化字符串的比较操作。
  mutable size_t hashValue = 0;

  /// @brief 标记是否已经计算过哈希值。
  /// @details 如果为true，表示hashValue已经计算过，避免重复计算。
  mutable bool hashed = false;

 public:
  explicit String(List<Byte>&& codeUnits, size_t hashValue)
    : codeUnits(std::move(codeUnits)), hashValue(hashValue), hashed(true) {}
  explicit String(List<Byte>&& codeUnits) : codeUnits(std::move(codeUnits)) {}

  String(const String& other) = default;
  String(String&& other) noexcept = default;
  String& operator=(const String& other) = default;
  String& operator=(String&& other) noexcept = default;
  ~String() = default;

  [[nodiscard]] Byte GetCodeUnit(Index index) const noexcept {
    return codeUnits[index];
  }
  [[nodiscard]] Index GetCodeUnitCount() const noexcept {
    return codeUnits.Size();
  }
  [[nodiscard]] Unicode GetCodePoint(Index index) {
    if (index >= codePoints.Size()) {
      ParseCodePointAt(index);
    }
    return codePoints[index];
  }
  [[nodiscard]] Index GetCodePointCount() const noexcept {
    ParseCodePoints();
    return codePoints.Size();
  }

  /// 不依赖随机访问
  [[nodiscard]] bool StartsWith(const String& prefix) const;
  [[nodiscard]] bool EndsWith(const String& suffix) const;
  std::string ToCppString() const;
  String Copy() const { return String(codeUnits.Copy(), hashValue); }
  String Add(const String& rhs) const {
    return String(codeUnits.Copy().Add(rhs.codeUnits));
  }
  List<Byte> CopyCodeUnits() const { return codeUnits.Copy(); }

  /// 依赖随机访问
  [[nodiscard]] String Slice(Index start, Index end);
  [[nodiscard]] bool Equal(const String& rhs) noexcept;
  [[nodiscard]] bool GreaterThan(const String& rhs) noexcept;
  [[nodiscard]] bool LessThan(const String& rhs) noexcept {
    return !GreaterThan(rhs) && !Equal(rhs);
  }
  [[nodiscard]] bool GreaterThanOrEqual(const String& rhs) noexcept {
    return GreaterThan(rhs) || Equal(rhs);
  }
  [[nodiscard]] bool LessThanOrEqual(const String& rhs) noexcept {
    return !GreaterThan(rhs);
  }
  [[nodiscard]] bool NotEqual(const String& rhs) noexcept {
    return !Equal(rhs);
  }
  size_t HashValue() const;
  String Upper();
};

class StringBuilder {
 private:
  List<Byte> codeUnits;

 public:
  explicit StringBuilder() = default;
  explicit StringBuilder(const String& str) : codeUnits(str.codeUnits) {}
  void Append(const String& str) { codeUnits.Concat(str.codeUnits); }
  void Append(const Byte& codePoint) { codeUnits.Push(codePoint); }
  [[nodiscard]] String ToString() { return String(std::move(codeUnits)); }
  [[nodiscard]] Index Size() const { return codeUnits.Size(); }
  void Clear() { codeUnits.Clear(); }
};

}  // namespace kaubo::Collections
