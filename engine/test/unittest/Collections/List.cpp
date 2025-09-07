// NOLINTBEGIN(*)
#include "../test_default.h"

#include "Collections/List.h"

using namespace kaubo::Collections;
using kaubo::Index;
TEST(List, Add) {
  List<char> list1 = {1, 2, 3};
  List<char> list2 = {4, 5, 6};
  List<char> result = list1.Add(list2);
  // 验证结果
  ASSERT_EQ(result.Size(), 6);  // 检查大小是否为 6
  ASSERT_EQ(result[0], 1);      // 检查第一个元素
  ASSERT_EQ(result[1], 2);      // 检查第二个元素
  ASSERT_EQ(result[2], 3);      // 检查第三个元素
  ASSERT_EQ(result[3], 4);      // 检查第四个元素
  ASSERT_EQ(result[4], 5);      // 检查第五个元素
  ASSERT_EQ(result[5], 6);      // 检查第六个元素
}
TEST(List, Copy) {
  List<char> list = {1, 2, 3};
  List<char> result = list.Copy();
  // 验证结果
  ASSERT_EQ(result.Size(), 3);      // 检查大小是否为 3
  ASSERT_EQ(result.Capacity(), 3);  // 检查容量是否为 3
  ASSERT_EQ(result[0], 1);          // 检查第一个元素
  ASSERT_EQ(result[1], 2);          // 检查第二个元素
  ASSERT_EQ(result[2], 3);          // 检查第三个元素
}
TEST(List, Constructor) {
  const char* message = "Hello, World!";
  List<char> list2(strlen(message), 'a');
  for (size_t i = 0; i < strlen(message); i++) {
    ASSERT_EQ(list2[i], 'a');  // 检查每个元素是否正确
  }
}
TEST(List, First) {
  List<char> list = {1, 2, 3};
  // 验证结果
  ASSERT_EQ(list.First(), 1);  // 检查第一个元素是否为 1
  char element = list.Shift();
  ASSERT_EQ(element, 1);      // 检查移除的元素是否为 1
  ASSERT_EQ(list.Size(), 2);  // 检查大小是否为 2
  ASSERT_EQ(list[0], 2);      // 检查第一个元素是否为 2
  list.Unshift('R');
  ASSERT_EQ(list.Size(), 3);  // 检查大小是否为 3
  ASSERT_EQ(list[0], 'R');    // 检查第一个元素是否为 114514
  list.Clear();
  ASSERT_THROW(list.First(), std::runtime_error);  // 检查是否抛出异常
  ASSERT_THROW(list.Shift(), std::runtime_error);  // 检查是否抛出异常
  list.TrimExcess();
  list.Unshift('X');
  ASSERT_EQ(list.Size(), 1);  // 检查大小是否为 1
  list.Unshift('Y');
  ASSERT_EQ(list.Capacity(), 10);  // 检查容量是否为 2
}
// 测试自赋值
TEST(List, SelfAssignment) {
  List<char> list{1, 2, 3};
  list = std::move(list);
  EXPECT_EQ(list.Size(), 3);
  EXPECT_EQ(list[0], 1);
  EXPECT_EQ(list[1], 2);
  EXPECT_EQ(list[2], 3);
}
// 测试不同大小的列表
TEST(List, DifferentSizes) {
  List<char> list1{1, 2, 3};
  List<char> list2{4, 5, 6, 7, 8};
  list1 = std::move(list2);
  EXPECT_EQ(list1.Size(), 5);
  EXPECT_EQ(list1[0], 4);
  EXPECT_EQ(list1[1], 5);
  EXPECT_EQ(list1[2], 6);
  EXPECT_EQ(list1[3], 7);
  EXPECT_EQ(list1[4], 8);
  EXPECT_EQ(list2.Size(), 0);
  EXPECT_EQ(list2.Capacity(), 0);
}
// 测试空列表
TEST(List, EmptyList) {
  List<char> list1{1, 2, 3};
  List<char> list2(0);
  list1 = std::move(list2);
  EXPECT_EQ(list1.Size(), 0);
  EXPECT_EQ(list1.Capacity(), 0);
  EXPECT_EQ(list2.Size(), 0);
  EXPECT_EQ(list2.Capacity(), 0);
}
// 测试非空列表
TEST(List, NonEmptyList) {
  List<char> list1;
  List<char> list2{4, 5, 6, 7, 8};
  list1 = std::move(list2);
  EXPECT_EQ(list1.Size(), 5);
  EXPECT_EQ(list1[0], 4);
  EXPECT_EQ(list1[1], 5);
  EXPECT_EQ(list1[2], 6);
  EXPECT_EQ(list1[3], 7);
  EXPECT_EQ(list1[4], 8);
  EXPECT_EQ(list2.Size(), 0);
  EXPECT_EQ(list2.Capacity(), 0);
}
// 测试资源管理
TEST(List, ResourceManagement) {
  List<char> list1{1, 2, 3};
  List<char> list2{4, 5, 6, 7, 8};
  list1 = std::move(list2);
  EXPECT_EQ(list1.Size(), 5);
  EXPECT_EQ(list1.Capacity(), 5);
  EXPECT_EQ(list2.Size(), 0);
  EXPECT_EQ(list2.Capacity(), 0);
}
TEST(List, Set) {
  List<char> list{1, 2, 3};
  list.Set(0, 4);
  EXPECT_EQ(list[0], 4);
  list.Set(1, 5);
  EXPECT_EQ(list[1], 5);
  list.Set(2, 6);
  EXPECT_EQ(list[2], 6);
  ASSERT_THROW(list.Set(3, 7), std::runtime_error);
}
TEST(List, Get) {
  List<char> list{1, 2, 3};
  EXPECT_EQ(list.Get(0), 1);
  EXPECT_EQ(list.Get(1), 2);
  EXPECT_EQ(list.Get(2), 3);
  ASSERT_THROW(list.Get(3), std::out_of_range);
}
TEST(List, TrimExcess) {
  List<char> list{1, 2, 3};
  list.TrimExcess();
  EXPECT_EQ(list.Capacity(), 3);
  list.Push(4);
  list.Push(5);
  list.Push(6);
  list.TrimExcess();
  EXPECT_EQ(list.Capacity(), 6);
}
TEST(List, Reverse) {
  List<char> list{1, 2, 3};
  list.Reverse();
  EXPECT_EQ(list[0], 3);
  EXPECT_EQ(list[1], 2);
  EXPECT_EQ(list[2], 1);
}
TEST(List, RemoveRange) {
  List<char> list{1, 2, 3, 4, 5, 6};
  list.RemoveRange(1, 3);
  EXPECT_EQ(list.Size(), 3);
  EXPECT_EQ(list[0], 1);
  EXPECT_EQ(list[1], 5);
  EXPECT_EQ(list[2], 6);
  ASSERT_THROW(list.RemoveRange(1, 4), std::runtime_error);
}
TEST(List, RemoveAt) {
  List<char> list{1, 2, 3, 4, 5, 6};
  list.RemoveAt(1);
  EXPECT_EQ(list.Size(), 5);
  EXPECT_EQ(list[0], 1);
  EXPECT_EQ(list[1], 3);
  EXPECT_EQ(list[2], 4);
  EXPECT_EQ(list[3], 5);
  EXPECT_EQ(list[4], 6);
  ASSERT_THROW(list.RemoveAt(5), std::runtime_error);
}
TEST(List, Slice) {
  List<char> list{1, 2, 3, 4, 5, 6};
  List<char> sub = list.Slice(1, 4);
  EXPECT_EQ(sub.Size(), 3);
  EXPECT_EQ(sub[0], 2);
  EXPECT_EQ(sub[1], 3);
  EXPECT_EQ(sub[2], 4);
  ASSERT_THROW(list.Slice(1, 7), std::runtime_error);
}
TEST(List, Clear) {
  List<char> list{1, 2, 3};
  list.Clear();
  EXPECT_EQ(list.Size(), 0);
}
TEST(List, Concat) {
  List<char> list1{1, 2, 3};
  List<char> list2{4, 5, 6};
  list1.Concat(list2);
  EXPECT_EQ(list1.Size(), 6);
  EXPECT_EQ(list1[0], 1);
  EXPECT_EQ(list1[1], 2);
  EXPECT_EQ(list1[2], 3);
  EXPECT_EQ(list1[3], 4);
  EXPECT_EQ(list1[4], 5);
  EXPECT_EQ(list1[5], 6);
}
TEST(List, Move) {
  List<char> list1{1, 2, 3};
  List<char> list2(std::move(list1));
  EXPECT_EQ(list2.Size(), 3);
}
TEST(List, Last) {
  List<char> list{1, 2, 3};
  EXPECT_EQ(list.Last(), 3);
  char element = list.Pop();
  EXPECT_EQ(element, 3);
  EXPECT_EQ(list.Size(), 2);
  EXPECT_EQ(list[0], 1);
  list.Push(4);
  EXPECT_EQ(list.Size(), 3);
  EXPECT_EQ(list[2], 4);
  list.Clear();
  ASSERT_THROW(list.Last(), std::runtime_error);
  ASSERT_THROW(list.Pop(), std::runtime_error);
}
TEST(List, Fill) {
  List<char> list(10);
  list.Fill(1);
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(list[i], 1);
  }
  List<char> list2 = list.Slice(0, 5).Copy();
  list2.Fill(2);
  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(list2[i], 2);
  }
  List<char> list3(0);
  ASSERT_THROW(list3.Fill(1), std::runtime_error);
}

TEST(List, ConstOperatorIndex) {
  const List<char> list{1, 2, 3};
  ASSERT_EQ(list[0], 1);  // 使用const版本的operator[]访问元素
  ASSERT_EQ(list[1], 2);
  ASSERT_EQ(list[2], 3);
  ASSERT_THROW(list[3], std::out_of_range);  // 测试越界访问
}
TEST(List, RemoveAtBoundaryAndError) {
  List<char> list{1, 2, 3};
  list.RemoveAt(0);  // 移除第一个元素
  ASSERT_EQ(list.Size(), 2);
  ASSERT_EQ(list[0], 2);
  ASSERT_EQ(list[1], 3);
  list.RemoveAt(list.Size() - 1);  // 移除最后一个元素
  ASSERT_EQ(list.Size(), 1);
  ASSERT_EQ(list[0], 2);
  ASSERT_THROW(list.RemoveAt(-1), std::runtime_error);           // 测试负数索引
  ASSERT_THROW(list.RemoveAt(list.Size()), std::runtime_error);  // 测试索引越界
}
TEST(List, RemoveRangeBoundaryAndError) {
  List<char> list{1, 2, 3, 4, 5};
  list.RemoveRange(0, 1);  // 移除第一个元素
  ASSERT_EQ(list.Size(), 4);
  ASSERT_EQ(list[0], 2);
  list.RemoveRange(list.Size() - 2, 2);  // 移除最后两个元素
  ASSERT_EQ(list.Size(), 2);
  ASSERT_EQ(list[0], 2);
  ASSERT_EQ(list[1], 3);
  ASSERT_THROW(
    list.RemoveRange(-1, 2), std::runtime_error
  );  // 测试负数起始索引
  ASSERT_THROW(
    list.RemoveRange(0, list.Size() + 1), std::runtime_error
  );  // 测试结束索引越界
  ASSERT_THROW(
    list.RemoveRange(2, 1), std::runtime_error
  );  // 测试起始索引大于结束索引
}
TEST(List, GetAndOperatorIndexError) {
  List<char> list{1, 2, 3};
  ASSERT_THROW(list.Get(-1), std::out_of_range);           // 测试负数索引
  ASSERT_THROW(list.Get(list.Size()), std::out_of_range);  // 测试索引越界
  ASSERT_THROW(list[-1], std::out_of_range);               // 测试负数索引
  ASSERT_THROW(list[list.Size()], std::out_of_range);      // 测试索引越界
}
TEST(List, ConcatAndReverse) {
  List<char> list1{1, 2, 3};
  List<char> list2{4, 5, 6};
  list1.Concat(list2);  // 合并
  list1.Reverse();      // 反转
  ASSERT_EQ(list1.Size(), 6);
  ASSERT_EQ(list1[0], 6);
  ASSERT_EQ(list1[1], 5);
  ASSERT_EQ(list1[2], 4);
  ASSERT_EQ(list1[3], 3);
  ASSERT_EQ(list1[4], 2);
  ASSERT_EQ(list1[5], 1);
}
TEST(List, AddAndCopyAndConcat) {
  List<char> list1{1, 2, 3};
  List<char> list2{4, 5, 6};
  List<char> result = list1.Add(list2);
  list2 = result.Copy();
  list1.Concat(list2);
  ASSERT_EQ(list1.Size(), 9);
  ASSERT_EQ(list1[0], 1);
  ASSERT_EQ(list1[1], 2);
  ASSERT_EQ(list1[2], 3);
  ASSERT_EQ(list1[3], 1);
  ASSERT_EQ(list1[4], 2);
  ASSERT_EQ(list1[5], 3);
  ASSERT_EQ(list1[6], 4);
  ASSERT_EQ(list1[7], 5);
  ASSERT_EQ(list1[8], 6);
}

TEST(List, InsertAndReplace) {
  List<char> list{1, 2, 3, 4, 5};
  List<char> insertList{6, 7, 8};
  list.InsertAndReplace(1, 3, insertList);
  ASSERT_EQ(list.Size(), 6);
  ASSERT_EQ(list[0], 1);
  ASSERT_EQ(list[1], 6);
  ASSERT_EQ(list[2], 7);
  ASSERT_EQ(list[3], 8);
  ASSERT_EQ(list[4], 4);
  ASSERT_EQ(list[5], 5);
}

// NOLINTEND(*)