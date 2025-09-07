#include "Collections/Integer/Integer.h"
#include "Collections/Integer/Decimal.h"
#include "Collections/Integer/DecimalHelper.h"
#include "Collections/Integer/IntegerHelper.h"
#include "Collections/String/StringHelper.h"

namespace kaubo::Collections {
Integer::Integer(const List<uint32_t>& _parts, bool _sign)
  : parts(_parts), sign(_sign) {}
Integer::Integer() = default;
bool Integer::Sign() const {
  return sign;
}
Integer::IntSign Integer::GetSign() const {
  return sign ? IntSign::Negative : IntSign::Positive;
}
String Integer::ToString() const {
  return CreateDecimalWithInteger(*this).ToString();
}
String Integer::ToHexString() const {
  if (IsZero()) {
    return CreateStringWithCString("0x0");
  }

  List<Byte> str;

  for (Index i = 0; i < parts.Size(); ++i) {
    uint32_t item = parts.Get(i);

    // 每个 32 位整数可以拆分为 8 个十六进制字符
    constexpr uint8_t NIBBLE_BITS = 4;     // 每个十六进制位占4位
    constexpr uint8_t NIBBLE_MASK = 0x0F;  // 用于提取每个 nibble
    for (auto j = NIBBLE_BITS - 1; j >= 0; --j) {
      // 使用无符号类型进行位运算
      auto nibble =
        static_cast<uint8_t>((item >> (j * NIBBLE_BITS)) & NIBBLE_MASK);

      Byte hex_char = (nibble < Decimal::radix)
                        ? (Byte_0 + nibble)
                        : (Byte_A + nibble - Decimal::radix);

      str.Push(hex_char);
    }
  }

  // 去除前导零
  Index iter = 0;
  while (iter < str.Size() && str.Get(iter) == Byte_0) {
    ++iter;
  }
  str = str.Slice(iter, str.Size());

  // 添加 "0x" 前缀
  str.Insert(0, 'x');
  str.Insert(0, '0');

  return String(std::move(str));
}
Integer Integer::Add(const Integer& rhs) const {  // NOLINT(misc-no-recursion)
  if (sign == rhs.sign) {
    uint32_t carry = 0;
    List<uint32_t> result;
    for (Index i = parts.Size() - 1, j = rhs.parts.Size() - 1;
         ((~i) != 0U) || ((~j) != 0U);) {
      uint32_t sum = carry;
      if ((~i) != 0U) {
        sum += parts.Get(i);
        --i;
      }
      if ((~j) != 0U) {
        sum += rhs.parts.Get(j);
        --j;
      }
      // 进位是高16位
      carry = (sum >> significantBits) & low16Mask;
      sum &= low16Mask;
      result.Push(sum);
    }
    if (carry != 0) {
      result.Push(carry);
    }
    result.Reverse();
    return Integer(result, sign);
  }
  return Subtract(Integer(rhs.parts, !rhs.sign));
}

Integer Integer::Copy() const {
  Integer newInteger;
  newInteger.parts = parts.Copy();
  newInteger.sign = sign;
  return newInteger;
}
bool Integer::GreaterThan(const Integer& rhs) const {
  bool lzero = IsZero();
  bool rzero = rhs.IsZero();
  if (lzero && rzero) {
    return false;
  }
  if (lzero) {
    return rhs.sign;
  }
  if (rzero) {
    return !sign;
  }
  // 如果左值是负数，右值是正数，那么左值一定比右值小
  if (sign && !rhs.sign) {
    return false;
  }
  // 如果左值是正数，右值是负数，那么左值一定比右值大
  if (!sign && rhs.sign) {
    return true;
  }
  // 此时左值和右值正负号相同
  if (parts.Size() != rhs.parts.Size()) {
    // 正号，位数多的数大；负号，位数少的数大
    return sign ^ (parts.Size() > rhs.parts.Size());
  }
  // 此时左值和右值的位数相等，正负号相同
  for (Index i = 0; i < parts.Size(); i++) {
    if (parts.Get(i) == rhs.parts.Get(i)) {
      continue;
    }
    return sign ^ (parts.Get(i) > rhs.parts.Get(i));
  }
  return false;
}
Integer Integer::Subtract(  // NOLINT(misc-no-recursion)
  const Integer& rhs
) const {
  // 正负号相同
  if (sign == rhs.sign) {
    // 假定左值大于右值
    bool _sign = false;
    Index size = 0;
    List<uint32_t> _lhs = parts.Copy();
    _lhs.Reverse();
    List<uint32_t> _rhs = rhs.parts.Copy();
    _rhs.Reverse();
    if (_lhs.Size() > _rhs.Size()) {
      _rhs.Concat(
        List<uint32_t>(_lhs.Size() - _rhs.Size(), static_cast<uint32_t>(0))
      );
      size = _lhs.Size();
    } else if (_lhs.Size() < _rhs.Size()) {
      _lhs.Concat(
        List<uint32_t>(_rhs.Size() - _lhs.Size(), static_cast<uint32_t>(0))
      );
      size = _rhs.Size();
      _sign = true;
      std::swap(_lhs, _rhs);
    } else {
      size = _lhs.Size();
      for (Index i = size - 1; (~i) != 0U; --i) {
        if (_lhs.Get(i) > _rhs.Get(i)) {
          break;
        }
        if (_lhs.Get(i) < _rhs.Get(i)) {
          _sign = true;
          std::swap(_lhs, _rhs);
          break;
        }
      }
    }
    List<uint32_t> result(size);
    bool borrow = false;
    for (Index i = 0; i < size; i++) {
      uint32_t diff = 0;
      uint32_t sub = _rhs.Get(i) + (borrow ? 1 : 0);
      if (_lhs.Get(i) < sub) {
        diff = low16Mask + 1 + _lhs.Get(i) - sub;
        borrow = true;
      } else {
        diff = _lhs.Get(i) - sub;
        borrow = false;
      }
      result.Push(diff);
    }
    TrimTrailingZero(result);
    result.Reverse();
    return Integer(result, _sign ^ sign);
  }
  return Add(Integer(rhs.parts, !rhs.sign));
}
Integer Integer::Multiply(const Integer& rhs) const {
  if (IsZero() || rhs.IsZero()) {
    return CreateIntegerZero();
  }
  List<uint32_t> result(parts.Size() + rhs.parts.Size() + 1);
  result.Fill(0);
  auto _lhs = parts;
  _lhs.Reverse();
  auto _rhs = rhs.parts;
  _rhs.Reverse();
  for (Index i = 0; i < _lhs.Size(); i++) {
    for (Index j = 0; j < _rhs.Size(); j++) {
      uint32_t product = _lhs.Get(i) * _rhs.Get(j);
      Index index = i + j;
      result.Set(index, result.Get(index) + product);
      result.Set(
        index + 1, result.Get(index + 1) + (result.Get(index) >> Integer::radix)
      );
      result.Set(index, (result.Get(index) & low16Mask));
    }
  }
  TrimTrailingZero(result);
  result.Reverse();
  return Integer(result, sign ^ rhs.sign);
}
Integer Integer::Divide(const Integer& rhs) const {
  return DivMod(rhs).Get(0);
}
Integer Integer::Modulo(const Integer& rhs) const {
  return DivMod(rhs).Get(1);
}
List<Integer> Integer::DivMod(const Integer& rhs) const {
  if (rhs.IsZero()) {
    throw std::runtime_error("Division by zero");
  }
  if (IsZero() || LessThan(rhs)) {
    return List<Integer>({CreateIntegerZero(), Copy()});
  }
  if (Equal(rhs)) {
    return List<Integer>({CreateIntegerOne(), CreateIntegerZero()});
  }
  Integer result;
  result.sign = sign ^ rhs.sign;
  Integer dividend =
    Slice(*this, 0, std::min(parts.Size(), rhs.parts.Size())).Copy();
  dividend.sign = false;
  Integer divisor = rhs.Copy();
  divisor.sign = false;
  for (Index i = 0; i < parts.Size() - rhs.parts.Size() + 1; i++) {
    uint32_t quotient = 0;
    while (dividend.GreaterThanOrEqual(divisor)) {
      dividend = dividend.Subtract(divisor);
      quotient++;
    }
    result.parts.Push(quotient);
    if (i + rhs.parts.Size() < parts.Size()) {
      dividend.parts.Push(parts.Get(i + rhs.parts.Size()));
    }
    TrimLeadingZero(dividend.parts);
  }
  List<Integer> divmods;
  TrimTrailingZero(result.parts);
  TrimTrailingZero(dividend.parts);
  divmods.Push(result);
  divmods.Push(dividend);
  return divmods;
}
bool Integer::IsZero() const {
  if (parts.Size() == 0) {
    return true;
  }
  for (Index i = 0; i < parts.Size(); i++) {
    if (parts.Get(i) != 0) {
      return false;
    }
  }
  return true;
}
bool Integer::Equal(const Integer& rhs) const {
  if (IsZero() && rhs.IsZero()) {
    return true;
  }
  if (sign != rhs.sign) {
    return false;
  }
  if (parts.Size() != rhs.parts.Size()) {
    return false;
  }
  for (Index i = 0; i < parts.Size(); i++) {
    if (parts.Get(i) != rhs.parts.Get(i)) {
      return false;
    }
  }
  return true;
}
Integer Integer::BitWiseAnd(const Integer& rhs) const {
  List<uint32_t> result(std::max(parts.Size(), rhs.parts.Size()));
  for (Index i = parts.Size() - 1, j = rhs.parts.Size() - 1;
       ((~i) != 0U) || ((~j) != 0U); --i, --j) {
    if (((~i) == 0U) || ((~j) == 0U)) {
      result.Push(0);
    } else {
      result.Push(parts.Get(i) & rhs.parts.Get(j));
    }
  }
  result.Reverse();
  return Integer(result, sign ^ rhs.sign);
}
Integer Integer::BitWiseOr(const Integer& rhs) const {
  List<uint32_t> result(std::max(parts.Size(), rhs.parts.Size()));
  for (Index i = parts.Size() - 1, j = rhs.parts.Size() - 1;
       ((~i) != 0U) || ((~j) != 0U); --i, --j) {
    if ((~i) == 0U) {
      result.Push(rhs.parts[j]);
    } else if ((~j) == 0U) {
      result.Push(parts[i]);
    } else {
      result.Push(parts[i] | rhs.parts[j]);
    }
  }
  result.Reverse();
  return Integer(result, sign ^ rhs.sign);
}
Integer Integer::BitWiseXor(const Integer& rhs) const {
  List<uint32_t> result(std::max(parts.Size(), rhs.parts.Size()));
  for (Index i = parts.Size() - 1, j = rhs.parts.Size() - 1;
       ((~i) != 0U) || ((~j) != 0U); --i, --j) {
    if ((~i) == 0U) {
      result.Push(rhs.parts[j]);
    } else if ((~j) == 0U) {
      result.Push(parts[i]);
    } else {
      result.Push(parts[i] ^ rhs.parts[j]);
    }
  }
  result.Reverse();
  return Integer(result, sign ^ rhs.sign);
}
Integer Integer::BitWiseNot() const {
  List<uint32_t> result(parts.Size());
  for (Index i = 0; i < parts.Size(); i++) {
    result.Push(~parts.Get(i));
  }
  return Integer(result, !sign);
}
bool Integer::LessThan(const Integer& rhs) const {
  return !GreaterThan(rhs) && !Equal(rhs);
}
bool Integer::GreaterThanOrEqual(const Integer& rhs) const {
  return GreaterThan(rhs) || Equal(rhs);
}
bool Integer::LessThanOrEqual(const Integer& rhs) const {
  return !GreaterThan(rhs);
}
bool Integer::NotEqual(const Integer& rhs) const {
  return !Equal(rhs);
}
List<uint32_t> Integer::Data() const {
  return parts;
}

Integer Integer::Negate() const {
  return Integer(parts, !sign);
}

Integer Integer::Power(const Integer& rhs) const {
  if (rhs.sign) {
    throw std::runtime_error("Exponent must be non-negative");
  }
  if (rhs.IsZero()) {
    return CreateIntegerOne();
  }
  if (rhs.Equal(CreateIntegerOne())) {
    return Copy();
  }
  if (rhs.Equal(CreateIntegerTwo())) {
    return Multiply(*this);
  }
  Integer result = CreateIntegerOne();
  Integer base = Copy();
  Integer exponent = rhs.Copy();
  while (!exponent.IsZero()) {
    if ((exponent.parts.Get(0) & 1) != 0U) {
      result = result.Multiply(base);
    }
    base = base.Multiply(base);
    exponent = exponent.RightShift(CreateIntegerOne());
  }
  return result;
}

Integer Integer::LeftShift(const Integer& rhs) const {
  if (rhs.sign) {
    throw std::runtime_error("Shift count must be non-negative");
  }
  if (rhs.IsZero()) {
    return Copy();
  }

  // 计算总位移量（使用uint64_t防止32位溢出）
  uint64_t totalShift = ToU64(rhs);
  uint64_t blockShift = ((totalShift - 1) / Integer::radix) + 1;
  uint64_t bitShift = totalShift % Integer::radix;
  if (bitShift != 0) {
    bitShift = Integer::radix - bitShift;
  }
  Integer result = Copy();
  result.parts.ExpandWithElement(result.parts.Size() + blockShift, 0);
  if (bitShift == 0) {
    return result;
  }
  Integer IntBitShift = CreateIntegerWithU64(bitShift);
  return result.RightShift(IntBitShift);
}

Integer Integer::RightShift(const Integer& rhs) const {
  if (rhs.sign) {
    throw std::runtime_error("Shift count must be non-negative");
  }
  if (rhs.IsZero()) {
    return Copy();
  }
  // 计算总位移量
  uint64_t totalShift = ToU64(rhs);
  // 如果总位移量超过最大范围，直接返回0
  if (totalShift >= Integer::radix * parts.Size()) {
    return CreateIntegerZero();
  }
  Integer result = Copy();
  uint64_t blockShift = totalShift / Integer::radix;
  uint64_t bitShift = totalShift % Integer::radix;
  result.parts.RemoveRange(result.parts.Size() - 1 - blockShift, blockShift);
  if (bitShift == 0) {
    return result;
  }
  uint32_t overflowPicker = (1 << (bitShift + 1)) - 1;
  for (Index i = result.parts.Size() - 2; (~i) != 0U; i--) {
    uint32_t high = result.parts.Get(i);
    uint32_t low = result.parts.Get(i + 1);
    low >>= bitShift;
    high &= overflowPicker;
    high = (high << (Integer::radix - bitShift)) & low16Mask;
    result.parts.Set(i + 1, low | high);
  }
  result.parts.Set(0, result.parts.Get(0) >> bitShift);
  TrimLeadingZero(result.parts);
  return result;
}
}  // namespace kaubo::Collections