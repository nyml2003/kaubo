// NOLINTBEGIN(*)
#include "../test_default.h"

#include "Collections.h"

using namespace kaubo::Collections;

TEST(String, CreateStringWithCString) {
  String str = CreateStringWithCString("Hello, World!");
  ASSERT_EQ(str.GetCodeUnitCount(), 13);
  ASSERT_EQ(str.GetCodePointCount(), 13);
  ASSERT_EQ(str.ToCppString(), "Hello, World!");
}

TEST(String, ComplexString) {
  String str = CreateStringWithCString("O(∩_∩)O~~");
  ASSERT_EQ(str.GetCodePointCount(), 9);
  ASSERT_EQ(str.GetCodeUnitCount(), 13);
  ASSERT_EQ(str.ToCppString(), "O(∩_∩)O~~");
  String str2 =
    CreateStringWithCString("αββγδδδεεζηθãäèèÄゲグデバタヒɔ:aiwjd❉♂◙㊦㊨");
  ASSERT_EQ(str2.GetCodePointCount(), 35);
  ASSERT_EQ(str2.GetCodeUnitCount(), 75);
  ASSERT_EQ(str2.ToCppString(), "αββγδδδεεζηθãäèèÄゲグデバタヒɔ:aiwjd❉♂◙㊦㊨");
  String str3 = CreateStringWithCString("👨‍👩‍👧‍👦");
  ASSERT_EQ(str3.GetCodePointCount(), 7);
  ASSERT_EQ(str3.GetCodeUnitCount(), 25);
  ASSERT_EQ(str3.ToCppString(), "👨‍👩‍👧‍👦");
}

TEST(String, Concat) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString(" World");
  StringBuilder stringBuilder;
  stringBuilder.Append(str1);
  stringBuilder.Append(str2);
  String str3 = stringBuilder.ToString();
  ASSERT_EQ(str3.GetCodePointCount(), 11);
  String expected = CreateStringWithCString("Hello World");
  ASSERT_TRUE(str3.Equal(expected));
  ASSERT_TRUE(str2.GetCodeUnit(0) == ' ');
  ASSERT_TRUE(str2.GetCodeUnit(1) == 'W');
}

TEST(String, Push) {
  StringBuilder stringBuilder;
  stringBuilder.Append('H');
  stringBuilder.Append('e');
  stringBuilder.Append('l');
  stringBuilder.Append('l');
  stringBuilder.Append('o');
  ASSERT_EQ(stringBuilder.Size(), 5);
  String expected = CreateStringWithCString("Hello");
  ASSERT_TRUE(stringBuilder.ToString().Equal(expected));
}

TEST(String, MixedString) {
  String str = CreateStringWithCString("Hello, 世界");
  ASSERT_EQ(str.GetCodePointCount(), 9);
  ASSERT_EQ(str.GetCodeUnitCount(), 13);
  ASSERT_EQ(str.ToCppString(), "Hello, 世界");
  ASSERT_EQ(str.GetCodePoint(0), 'H');
  ASSERT_EQ(str.GetCodePoint(1), 'e');
  ASSERT_EQ(str.GetCodePoint(2), 'l');
  ASSERT_EQ(str.GetCodePoint(3), 'l');
  ASSERT_EQ(str.GetCodePoint(4), 'o');
  ASSERT_EQ(str.GetCodePoint(5), ',');
  ASSERT_EQ(str.GetCodePoint(6), ' ');
  ASSERT_EQ(str.GetCodePoint(7), 19990);
  ASSERT_EQ(str.GetCodePoint(8), 30028);
  ASSERT_EQ(str.GetCodeUnit(0), 'H');
  ASSERT_EQ(str.GetCodeUnit(1), 'e');
  ASSERT_EQ(str.GetCodeUnit(2), 'l');
  ASSERT_EQ(str.GetCodeUnit(3), 'l');
  ASSERT_EQ(str.GetCodeUnit(4), 'o');
  ASSERT_EQ(str.GetCodeUnit(5), ',');
  ASSERT_EQ(str.GetCodeUnit(6), ' ');
  ASSERT_EQ(str.GetCodeUnit(7), 0xe4);
  ASSERT_EQ(str.GetCodeUnit(8), 0xb8);
  ASSERT_EQ(str.GetCodeUnit(9), 0x96);
  ASSERT_EQ(str.GetCodeUnit(10), 0xe7);
  ASSERT_EQ(str.GetCodeUnit(11), 0x95);
  ASSERT_EQ(str.GetCodeUnit(12), 0x8c);
}

TEST(String, Slice) {
  String str = CreateStringWithCString("Hello World");
  String slicedStr = str.Slice(0, 5);
  ASSERT_EQ(slicedStr.GetCodePointCount(), 5);
  ASSERT_EQ(slicedStr.ToCppString(), "Hello");
  // error
  ASSERT_THROW(str.Slice(0, 12), std::out_of_range);
  ASSERT_THROW(str.Slice(0, 0), std::invalid_argument);
  ASSERT_THROW(str.Slice(5, 0), std::invalid_argument);
  ASSERT_THROW(str.Slice(5, 5), std::invalid_argument);
  ASSERT_THROW(str.Slice(5, 4), std::invalid_argument);

  ASSERT_EQ(str.Slice(0, 1).ToCppString(), "H");
  ASSERT_EQ(str.Slice(1, 2).ToCppString(), "e");
  ASSERT_EQ(str.Slice(2, 3).ToCppString(), "l");
  ASSERT_EQ(str.Slice(3, 4).ToCppString(), "l");
  ASSERT_EQ(str.Slice(4, 5).ToCppString(), "o");
  ASSERT_EQ(str.Slice(5, 6).ToCppString(), " ");
  ASSERT_EQ(str.Slice(6, 7).ToCppString(), "W");
  ASSERT_EQ(str.Slice(7, 8).ToCppString(), "o");
  ASSERT_EQ(str.Slice(8, 9).ToCppString(), "r");
  ASSERT_EQ(str.Slice(9, 10).ToCppString(), "l");
  ASSERT_EQ(str.Slice(10, 11).ToCppString(), "d");
  ASSERT_EQ(str.Slice(0, 11).ToCppString(), "Hello World");
}
//
//// TEST(String, Find) {
////   String str = CreateStringWithCString("Hello World");
////   String sub = CreateStringWithCString("ll");
////   ASSERT_EQ(str.Find(sub, 0), 2);
////   String empty = CreateStringWithCString("");
////   ASSERT_EQ(str.Find(empty, 0), 0);
////   ASSERT_THROW(str.Find(sub, 12), std::runtime_error);
////   ASSERT_THROW(str.Find(sub, 10), std::runtime_error);
////   str = CreateStringWithCString("abababab");
////   sub = CreateStringWithCString("bab");
////   ASSERT_EQ(str.Find(sub, 0), 1);
//// }
//
TEST(String, Equal) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString("Hello");
  ASSERT_TRUE(str1.Equal(str2));
}

TEST(String, GreaterThan) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString("Hello World");
  ASSERT_TRUE(str2.GreaterThan(str1));
  String str3 = CreateStringWithCString("Heaven");
  String str4 = CreateStringWithCString("Hello");
  ASSERT_TRUE(str4.GreaterThan(str3));
}

TEST(String, LessThan) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString("Hello World");
  ASSERT_TRUE(str1.LessThan(str2));
  String str3 = CreateStringWithCString("Heaven");
  String str4 = CreateStringWithCString("Hello");
  ASSERT_TRUE(str3.LessThan(str4));
}

TEST(String, GreaterThanOrEqual) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString("Hello");
  ASSERT_TRUE(str1.GreaterThanOrEqual(str2));
  String str3 = CreateStringWithCString("Heaven");
  String str4 = CreateStringWithCString("Hello");
  ASSERT_FALSE(str3.GreaterThanOrEqual(str4));
}

TEST(String, LessThanOrEqual) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString("Hello");
  ASSERT_TRUE(str1.LessThanOrEqual(str2));
  String str3 = CreateStringWithCString("Heaven");
  String str4 = CreateStringWithCString("Hello");
  ASSERT_TRUE(str3.LessThanOrEqual(str4));
}

TEST(String, NotEqual) {
  String str1 = CreateStringWithCString("Hello");
  String str2 = CreateStringWithCString("World");
  ASSERT_TRUE(str1.NotEqual(str2));
}

TEST(String, ToString) {
  double value = 3.14159;
  String str = ToString(value);
  ASSERT_TRUE(str.Equal(CreateStringWithCString("3.141590")));

  int32_t value2 = 123;
  String str2 = ToString(value2);
  ASSERT_TRUE(str2.Equal(CreateStringWithCString("123")));

  uint64_t value3 = 12345678901234567890;
  String str3 = ToString(value3);
  ASSERT_TRUE(str3.Equal(CreateStringWithCString("12345678901234567890")));

  int64_t value4 = -1234567890123456789;
  String str4 = ToString(value4);
  ASSERT_TRUE(str4.Equal(CreateStringWithCString("-1234567890123456789")));

  uint32_t value5 = 4294967295;
  String str5 = ToString(value5);
  ASSERT_TRUE(str5.Equal(CreateStringWithCString("4294967295")));
}
// TEST(String, Add) {
//  String str1 = CreateStringWithCString("Hello");
//  String str2 = CreateStringWithCString(" World");
//  String str3 = str1.Add(str2);
//  ASSERT_TRUE(str3.Equal(CreateStringWithCString("Hello World")));
//}
//
//// TEST(String, Spilt) {
////   String str = CreateStringWithCString("Hello, World");
////   String delimiter = CreateStringWithCString(", ");
////   kaubo::Collections::List<String> list = str.Split(delimiter);
////   ASSERT_EQ(list.Size(), 2);
////   ASSERT_TRUE(list.Get(0).Equal(CreateStringWithCString("Hello")));
////   ASSERT_TRUE(list.Get(1).Equal(CreateStringWithCString("World")));
////   str = CreateStringWithCString("h,e,l,l,o");
////   delimiter = CreateStringWithCString(",");
////   list = str.Split(delimiter);
////   ASSERT_EQ(list.Size(), 5);
////   ASSERT_TRUE(list.Get(0).Equal(CreateStringWithCString("h")));
////   ASSERT_TRUE(list.Get(1).Equal(CreateStringWithCString("e")));
////   ASSERT_TRUE(list.Get(2).Equal(CreateStringWithCString("l")));
////   ASSERT_TRUE(list.Get(3).Equal(CreateStringWithCString("l")));
////   ASSERT_TRUE(list.Get(4).Equal(CreateStringWithCString("o")));
//// }
//
// TEST(String, Move) {
//  String str1 = CreateStringWithCString("Hello");
//  String str2 = std::move(str1);
//  ASSERT_EQ(str1.Size(), 0);
//  ASSERT_EQ(str2.Size(), 5);
//  ASSERT_TRUE(str2.Equal(CreateStringWithCString("Hello")));
//  String str3(str2);
//  ASSERT_EQ(str2.Size(), 5);
//  ASSERT_EQ(str3.Size(), 5);
//  ASSERT_TRUE(str3.Equal(CreateStringWithCString("Hello")));
//}

// NOLINTEND(*)