// NOLINTBEGIN(*)
#include "../test_default.h"

#include "Collections.h"
#include "Collections/String/StringHelper.h"

using namespace kaubo::Collections;

TEST(Decimal, DecimalAddition) {
  Decimal a = CreateDecimalWithCString("123");
  Decimal b = CreateDecimalWithCString("456");
  Decimal c = a.Add(b);
  ASSERT_EQ(c.ToString().ToCppString(), "579");
}

TEST(Decimal, DecimalAdditionWithNegative) {
  Decimal a = CreateDecimalWithCString("-123");
  Decimal b = CreateDecimalWithCString("456");
  Decimal c = a.Add(b);
  ASSERT_EQ(c.ToString().ToCppString(), "333");
}

TEST(Decimal, DecimalSubtraction) {
  Decimal a = CreateDecimalWithCString("123");
  Decimal b = CreateDecimalWithCString("456");
  Decimal c = a.Subtract(b);
  ASSERT_EQ(c.ToString().ToCppString(), "-333");
}

TEST(Decimal, DecimalSubtractionWithNegative) {
  Decimal a = CreateDecimalWithCString("-123");
  Decimal b = CreateDecimalWithCString("-456");
  Decimal c = a.Subtract(b);
  ASSERT_EQ(c.ToString().ToCppString(), "333");
}

TEST(Decimal, DecimalMultiplication) {
  Decimal a = CreateDecimalWithCString("123");
  Decimal b = CreateDecimalWithCString("456");
  Decimal c = a.Multiply(b);
  ASSERT_EQ(c.ToString().ToCppString(), "56088");
}

TEST(Decimal, DecimalDivision) {
  Decimal a = CreateDecimalWithCString("123456");
  Decimal b = CreateDecimalWithCString("123");
  Decimal c = a.Divide(b);
  ASSERT_EQ(c.ToString().ToCppString(), "1003");
}

TEST(Decimal, DecimalModulo) {
  Decimal a = CreateDecimalWithCString("123456");
  Decimal b = CreateDecimalWithCString("123");
  Decimal c = a.Modulo(b);
  ASSERT_EQ(c.ToString().ToCppString(), "87");
  b = CreateDecimalWithCString("123456");
  c = a.Modulo(b);
  Decimal d = a.Divide(b);
  ASSERT_TRUE(c.IsZero());
  ASSERT_TRUE(d.Equal(CreateDecimalWithCString("1")));
}

TEST(Decimal, DecimalGreaterThan) {
  Decimal a = CreateDecimalWithCString("123");
  Decimal b = CreateDecimalWithCString("456");
  ASSERT_FALSE(a.GreaterThan(b));
  ASSERT_TRUE(b.GreaterThan(a));
}

TEST(Decimal, DecimalGreaterThanWithNegative) {
  Decimal a = CreateDecimalWithCString("-123");
  Decimal b = CreateDecimalWithCString("456");
  ASSERT_FALSE(a.GreaterThan(b));
  ASSERT_TRUE(b.GreaterThan(a));
}

TEST(Decimal, DecimalEqual) {
  Decimal a = CreateDecimalWithCString("123");
  Decimal b = CreateDecimalWithCString("123");
  ASSERT_TRUE(a.Equal(b));
}

TEST(Decimal, DecimalEqualWithNegative) {
  Decimal a = CreateDecimalWithCString("-123");
  Decimal b = CreateDecimalWithCString("-123");
  ASSERT_TRUE(a.Equal(b));
}

TEST(Decimal, DecimalZero) {
  Decimal a = CreateDecimalWithCString("0");
  ASSERT_TRUE(a.IsZero());
}

TEST(Decimal, DecimalDivisionByZero) {
  Decimal a = CreateDecimalWithCString("123");
  Decimal b = CreateDecimalWithCString("0");
  ASSERT_THROW(a.Divide(b), std::runtime_error);
}

TEST(Decimal, DecimalToHexString) {
  Decimal a = CreateDecimalWithCString("255");
  ASSERT_EQ(a.ToHexString().ToCppString(), "0xFF");
  a = CreateDecimalWithCString("123456789");
  ASSERT_EQ(a.ToHexString().ToCppString(), "0x75BCD15");
}

TEST(Decimal, ExtraAdd) {
  ASSERT_TRUE(CreateDecimalWithCString("1234567890")
                .Add(CreateDecimalWithCString("9876543210"))
                .Equal(CreateDecimalWithCString("11111111100")));
  ASSERT_TRUE(CreateDecimalWithCString("5678901234")
                .Add(CreateDecimalWithCString("4321098765"))
                .Equal(CreateDecimalWithCString("9999999999")));
  ASSERT_TRUE(CreateDecimalWithCString("1111111111")
                .Add(CreateDecimalWithCString("2222222222"))
                .Equal(CreateDecimalWithCString("3333333333")));
  ASSERT_TRUE(CreateDecimalWithCString("9999999999")
                .Add(CreateDecimalWithCString("1"))
                .Equal(CreateDecimalWithCString("10000000000")));
  ASSERT_TRUE(CreateDecimalWithCString("3333333333")
                .Add(CreateDecimalWithCString("6666666666"))
                .Equal(CreateDecimalWithCString("9999999999")));
  ASSERT_TRUE(CreateDecimalWithCString("1234567890")
                .Add(CreateDecimalWithCString("1234567890"))
                .Equal(CreateDecimalWithCString("2469135780")));
  ASSERT_TRUE(CreateDecimalWithCString("7777777777")
                .Add(CreateDecimalWithCString("2222222222"))
                .Equal(CreateDecimalWithCString("9999999999")));
  ASSERT_TRUE(CreateDecimalWithCString("4567890124")
                .Add(CreateDecimalWithCString("5432109876"))
                .Equal(CreateDecimalWithCString("10000000000")));
  ASSERT_TRUE(CreateDecimalWithCString("8888888888")
                .Add(CreateDecimalWithCString("1111111111"))
                .Equal(CreateDecimalWithCString("9999999999")));
  ASSERT_TRUE(CreateDecimalWithCString("2345678901")
                .Add(CreateDecimalWithCString("7654321098"))
                .Equal(CreateDecimalWithCString("9999999999")));
  ASSERT_TRUE(CreateDecimalWithCString("9876543210")
                .Subtract(CreateDecimalWithCString("1234567890"))
                .Equal(CreateDecimalWithCString("8641975320")));
  ASSERT_TRUE(CreateDecimalWithCString("5678901234")
                .Subtract(CreateDecimalWithCString("4321098765"))
                .Equal(CreateDecimalWithCString("1357802469")));

  ASSERT_TRUE(CreateDecimalWithCString("2222222222")
                .Subtract(CreateDecimalWithCString("1111111111"))
                .Equal(CreateDecimalWithCString("1111111111")));
  ASSERT_TRUE(CreateDecimalWithCString("9999999999")
                .Subtract(CreateDecimalWithCString("1"))
                .Equal(CreateDecimalWithCString("9999999998")));
  ASSERT_TRUE(CreateDecimalWithCString("6666666666")
                .Subtract(CreateDecimalWithCString("3333333333"))
                .Equal(CreateDecimalWithCString("3333333333")));
  ASSERT_TRUE(CreateDecimalWithCString("1234567890")
                .Subtract(CreateDecimalWithCString("1234567890"))
                .Equal(CreateDecimalWithCString("0")));
  ASSERT_TRUE(CreateDecimalWithCString("7777777777")
                .Subtract(CreateDecimalWithCString("2222222222"))
                .Equal(CreateDecimalWithCString("5555555555")));
  ASSERT_TRUE(CreateDecimalWithCString("5432109876")
                .Subtract(CreateDecimalWithCString("4567890123"))
                .Equal(CreateDecimalWithCString("864219753")));
  ASSERT_TRUE(CreateDecimalWithCString("8888888888")
                .Subtract(CreateDecimalWithCString("1111111111"))
                .Equal(CreateDecimalWithCString("7777777777")));
  ASSERT_TRUE(CreateDecimalWithCString("7654321098")
                .Subtract(CreateDecimalWithCString("2345678901"))
                .Equal(CreateDecimalWithCString("5308642197")));
}

TEST(Decimal, Zero) {
  ASSERT_TRUE(CreateDecimalWithCString("0").IsZero());
  ASSERT_EQ(CreateDecimalZero().ToString().ToCppString(), "0");
}

TEST(Decimal, LessThan) {
  ASSERT_TRUE(
    CreateDecimalWithCString("123").LessThan(CreateDecimalWithCString("456"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("456").LessThan(CreateDecimalWithCString("123"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("123").LessThan(CreateDecimalWithCString("123"))
  );
  ASSERT_TRUE(
    CreateDecimalWithCString("-123").LessThan(CreateDecimalWithCString("456"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("456").LessThan(CreateDecimalWithCString("-123"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("-123").LessThan(CreateDecimalWithCString("-123"))
  );
}

TEST(Decimal, LessThanOrEqual) {
  ASSERT_TRUE(CreateDecimalWithCString("123").LessThanOrEqual(
    CreateDecimalWithCString("456")
  ));
  ASSERT_FALSE(CreateDecimalWithCString("456").LessThanOrEqual(
    CreateDecimalWithCString("123")
  ));
  ASSERT_TRUE(CreateDecimalWithCString("123").LessThanOrEqual(
    CreateDecimalWithCString("123")
  ));
  ASSERT_TRUE(CreateDecimalWithCString("-123").LessThanOrEqual(
    CreateDecimalWithCString("456")
  ));
  ASSERT_FALSE(CreateDecimalWithCString("456").LessThanOrEqual(
    CreateDecimalWithCString("-123")
  ));
  ASSERT_TRUE(CreateDecimalWithCString("-123").LessThanOrEqual(
    CreateDecimalWithCString("-123")
  ));
  ASSERT_TRUE(
    CreateDecimalWithCString("0").LessThanOrEqual(CreateDecimalWithCString("1"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("1").LessThanOrEqual(CreateDecimalWithCString("0"))
  );
}

TEST(Decimal, NotEqual) {
  ASSERT_TRUE(
    CreateDecimalWithCString("123").NotEqual(CreateDecimalWithCString("456"))
  );
  ASSERT_TRUE(
    CreateDecimalWithCString("456").NotEqual(CreateDecimalWithCString("123"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("123").NotEqual(CreateDecimalWithCString("123"))
  );
  ASSERT_TRUE(
    CreateDecimalWithCString("-123").NotEqual(CreateDecimalWithCString("456"))
  );
  ASSERT_TRUE(
    CreateDecimalWithCString("456").NotEqual(CreateDecimalWithCString("-123"))
  );
  ASSERT_FALSE(
    CreateDecimalWithCString("-123").NotEqual(CreateDecimalWithCString("-123"))
  );
  ASSERT_TRUE(
    CreateDecimalWithCString("0").NotEqual(CreateDecimalWithCString("1"))
  );
  ASSERT_TRUE(
    CreateDecimalWithCString("1").NotEqual(CreateDecimalWithCString("0"))
  );
}

TEST(Decimal, Extra) {
  Decimal a = CreateDecimalWithCString("456456768578941657896468574896789");
  Decimal b = CreateDecimalWithCString("486546534");
  kaubo::Collections::List<Decimal> c = a.DivMod(b);
  ASSERT_TRUE(
    c.Get(0).Equal(CreateDecimalWithCString("938156448934731611707398"))
  );
  ASSERT_TRUE(c.Get(1).Equal(CreateDecimalWithCString("255838257")));
}
// NOLINTEND(*)