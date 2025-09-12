#pragma once

#include "Collections/String/BytesHelper.h"
#include "Collections/String/StringHelper.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Number/PyFloat.h"
#include "Object/Number/PyInteger.h"
#include "Object/Runtime/PyCode.h"
#include "Object/String/PyBytes.h"

#include <filesystem>
#include <fstream>

namespace kaubo::Runtime {
namespace fs = std::filesystem;

class BinaryFileParser {
 public:
  explicit BinaryFileParser(const fs::path& filename) {
    fileStream.open(filename, std::ios::binary);
    if (!fileStream.is_open()) {
      throw std::runtime_error("Failed to open file: " + filename.string());
    }
  }

  Object::PyCodePtr Parse() {
    auto code = ReadObject()->as<Object::PyCode>();
    fileStream.close();
    return code;
  }

 private:
  std::ifstream fileStream;
  uint64_t ReadSize() {
    std::streampos originalPosition = fileStream.tellg();
    uint64_t size = 0;
    fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
    fileStream.seekg(originalPosition);
    return size;
  }
  double ReadDouble() {
    double value = nan("");
    fileStream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
  }
  int64_t ReadI64() {
    int64_t value = 0;
    fileStream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
  }
  uint64_t ReadU64() {
    uint64_t value = 0;
    fileStream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
  }
  Object::PyStrPtr ReadString() {
    uint64_t size = ReadU64();
    auto buffer = std::make_unique<char[]>(size + 1);
    fileStream.read(
      reinterpret_cast<char*>(buffer.get()), static_cast<std::streamsize>(size)
    );
    return Object::PyString::Create(
      Collections::CreateStringWithCString(buffer.get())
    );
  }
  Collections::List<Byte> ReadBytes(uint64_t size) {
    auto buffer = std::make_unique<unsigned char[]>(size);
    fileStream.read(
      reinterpret_cast<char*>(buffer.get()), static_cast<std::streamsize>(size)
    );
    return Collections::List<Byte>(size, std::move(buffer));
  }
  Object::PyBytesPtr ReadBytes() {
    return Object::PyBytes::Create(Collections::String(ReadBytes(ReadU64())));
  }
  Object::PyIntPtr ReadInteger() {
    uint64_t size = ReadSize();
    return Object::CreatePyInteger(
      Collections::DeserializeInteger(
        ReadBytes((size * 2) + sizeof(uint64_t) + 1)
      )
    );
  }
  Object::PyFloatPtr ReadFloat() { return Object::CreatePyFloat(ReadDouble()); }
  Object::PyListPtr ReadList() {
    uint64_t size = ReadU64();
    if (size == 0) {
      return Object::PyList::Create(Object::PyList::ExpandOnly{0});
    }
    Collections::List<Object::PyObjPtr> list(size);
    for (uint64_t i = 0; i < size; ++i) {
      auto obj = ReadObject();
      list.Push(obj);
    }
    return Object::PyList::Create(list);
  }
  uint8_t ReadU8() {
    uint8_t value = 0;
    fileStream.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
  }
  Object::PyObjPtr ReadObject() {
    Byte type = ReadU8();
    auto literalEnum = static_cast<Object::Literal>(type);
    switch (literalEnum) {
      case Object::Literal::STRING:
        return ReadString();
      case Object::Literal::INTEGER:
        return ReadInteger();
      case Object::Literal::FLOAT:
        return ReadFloat();
      case Object::Literal::LIST:
        return ReadList();
      case Object::Literal::TRUE_LITERAL:
        return Object::PyBoolean::Create(true);
      case Object::Literal::FALSE_LITERAL:
        return Object::PyBoolean::Create(false);
      case Object::Literal::NONE:
        return Object::PyNone::Create();
      case Object::Literal::ZERO:
        return Object::CreatePyInteger(0ULL);
      case Object::Literal::CODE:
        return ReadCode();
      case Object::Literal::BYTES:
        return ReadBytes();
    }
    throw std::runtime_error("Unknown object type");
  }
  Object::PyCodePtr ReadCode() {
    auto consts = ReadObject()->as<Object::PyList>();
    auto names = ReadObject()->as<Object::PyList>();
    auto varNames = ReadObject()->as<Object::PyList>();
    auto name = ReadObject()->as<Object::PyString>();
    auto nLocals = ReadU64();
    auto isGenerator =
      static_cast<Object::Literal>(ReadU8()) == Object::Literal::TRUE_LITERAL;
    auto byteCode = ReadObject()->as<Object::PyBytes>();
    return std::make_shared<Object::PyCode>(
      byteCode, consts, names, varNames, name, nLocals, isGenerator
    );
  }
};
}  // namespace kaubo::Runtime
