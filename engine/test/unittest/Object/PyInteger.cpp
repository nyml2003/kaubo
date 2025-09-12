#include "Object/Number/PyInteger.h"
#include <memory>
// NOLINTBEGIN(*)
#include "../test_default.h"

#include "../Collections/Collections.h"
#include "ByteCode/ByteCode.h"
#include "Collections/String/BytesHelper.h"
#include "Object.h"
#include "Object/String/PyBytes.h"
#include "Object/String/PyString.h"

using namespace kaubo::Object;
using namespace kaubo::Collections;

namespace kaubo::Object {

class PyIntegerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 初始化操作，如创建测试对象等
    integer1 = std::dynamic_pointer_cast<PyInteger>(PyInteger::Create(10ULL));
    integer2 = std::dynamic_pointer_cast<PyInteger>(PyInteger::Create(20ULL));
    integer3 = std::dynamic_pointer_cast<PyInteger>(PyInteger::Create(10ULL));
  }

  PyIntPtr integer1;
  PyIntPtr integer2;
  PyIntPtr integer3;
};

TEST_F(PyIntegerTest, TestValue) {
  EXPECT_TRUE(integer1->Equal(PyInteger::Create(CreateIntegerWithCString("10")))
  );
  EXPECT_TRUE(integer2->Equal(PyInteger::Create(CreateIntegerWithCString("20")))
  );
}

TEST_F(PyIntegerTest, TestAdd) {
  auto result = IntegerKlass::Self()->add(integer1, integer2);
  EXPECT_TRUE(
    std::dynamic_pointer_cast<PyInteger>(result)->Equal(
      PyInteger::Create(CreateIntegerWithCString("30"))
    )
  );
}

TEST_F(PyIntegerTest, TestSub) {
  auto result = IntegerKlass::Self()->sub(integer2, integer1);
  EXPECT_TRUE(
    std::dynamic_pointer_cast<PyInteger>(result)->Equal(
      PyInteger::Create(CreateIntegerWithCString("10"))
    )
  );
}

TEST_F(PyIntegerTest, TestMul) {
  auto result = IntegerKlass::Self()->mul(integer1, integer2);
  EXPECT_TRUE(
    std::dynamic_pointer_cast<PyInteger>(result)->Equal(
      PyInteger::Create(CreateIntegerWithCString("200"))
    )
  );
}

TEST_F(PyIntegerTest, TestFloorDiv) {
  auto result = IntegerKlass::Self()->floordiv(integer2, integer1);
  EXPECT_TRUE(
    std::dynamic_pointer_cast<PyInteger>(result)->Equal(
      PyInteger::Create(CreateIntegerWithCString("2"))
    )
  );
}

TEST_F(PyIntegerTest, TestRepr) {
  auto result = IntegerKlass::Self()->repr(integer1);
  EXPECT_TRUE(
    std::dynamic_pointer_cast<PyString>(result)->Equal(PyString::Create("10"))
  );
}

TEST_F(PyIntegerTest, TestGt) {
  auto result = IntegerKlass::Self()->gt(integer2, integer1);
  EXPECT_TRUE(std::dynamic_pointer_cast<PyBoolean>(result)->Value());
}

TEST_F(PyIntegerTest, TestLt) {
  auto result = IntegerKlass::Self()->lt(integer1, integer2);
  EXPECT_TRUE(std::dynamic_pointer_cast<PyBoolean>(result)->Value());
}

TEST_F(PyIntegerTest, TestEq) {
  auto result = IntegerKlass::Self()->eq(integer1, integer3);
  EXPECT_TRUE(std::dynamic_pointer_cast<PyBoolean>(result)->Value());
}

TEST_F(PyIntegerTest, TestGe) {
  auto result = IntegerKlass::Self()->ge(integer2, integer1);
  EXPECT_TRUE(std::dynamic_pointer_cast<PyBoolean>(result)->Value());
}

TEST_F(PyIntegerTest, TestLe) {
  auto result = IntegerKlass::Self()->le(integer1, integer2);
  EXPECT_TRUE(std::dynamic_pointer_cast<PyBoolean>(result)->Value());
}

TEST_F(PyIntegerTest, TestNe) {
  auto result = IntegerKlass::Self()->ne(integer1, integer2);
  EXPECT_TRUE(std::dynamic_pointer_cast<PyBoolean>(result)->Value());
}

TEST_F(PyIntegerTest, TestSerialize) {
  auto result = IntegerKlass::Self()->_serialize_(integer1);
  const auto& bytes = result->as<PyBytes>()->Value();
  EXPECT_EQ(bytes.GetCodeUnit(0), static_cast<Byte>(Literal::INTEGER));
  String content(bytes.CopyCodeUnits().Slice(1, bytes.GetCodeUnitCount()));
  EXPECT_TRUE(
    Collections::DeserializeInteger(content.CopyCodeUnits())
      .Equal(CreateIntegerWithCString("10"))
  );
}

}  // namespace kaubo::Object

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
// NOLINTEND(*)