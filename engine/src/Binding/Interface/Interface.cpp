#include "Interface.h"
#include "Binding/Interface/impl.h"
#include "Collections/String/BytesHelper.h"
#include "Generation/Generator.h"
#include "IR/IRHelper.h"
#include "Lexer/Builder.h"
#include "Object/Core/CoreHelper.h"
#include "Parser/Parser.h"
#include "Runtime/BinaryFileParser.h"
#include "Runtime/VirtualMachine.h"
#include "Tools/Config/Config.h"
#include "Tools/Terminal/IntermediateRepresentationTerminal.h"
#include "Tools/Terminal/Terminal.h"
#include "Tools/Terminal/VerboseTerminal.h"


#include <sstream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif
namespace kaubo {
void init_runtime_support() {
  Object::LoadBootstrapClasses();
  Object::LoadRuntimeSupportClasses();
  IR::RegisterIRClasses();
}

void force_utf8() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);  // 在Windows平台上设置控制台输出为UTF-8编码
#endif
}

void handle_mode_compile() {
  auto code = compile(CreateInput());
  if (Config::has("file")) {
    auto data = code->_serialize_()->as<kaubo::Object::PyBytes>();
    const auto& bytes = data->Value();
    auto filePath = Config::get("file");
    auto writePath = std::filesystem::path(filePath).replace_extension(".code");
    Collections::Write(bytes, writePath.string());
  }
}

void handle_mode_interpret() {
  auto code = compile(CreateInput());
  interpret(code);
}

void handle_mode_interpret_bytecode() {
  auto filename = Config::get("file");
  kaubo::Runtime::BinaryFileParser parser(filename);
  auto code = parser.Parse();
  interpret(code);
}

auto CreateInput() -> std::string {
  if (Config::has("file")) {
    ConsoleTerminal::get_instance().debug("文件名: " + Config::get("file"));
    std::ifstream file(Config::get("file"));
    if (file.is_open()) {
      std::stringstream buffer;
      buffer << file.rdbuf();
      return buffer.str();
    }
  }
  if (Config::has("source")) {
    return Config::get("source");
  }
  throw std::runtime_error("未指定文件或源码");
}

auto compile(std::string source) -> Object::PyCodePtr {
  auto lexer = Lexer::Builder::get_instance();
  lexer->feed(source);
  lexer->terminate();
  Parser::Parser parser(std::move(lexer));
  auto ast = parser.parse();
  if (ast.is_err()) {
    throw std::runtime_error(std::to_string(ast.unwrap_err()));
  }

  const std::string moduleName = []() {
    if (Config::has("file")) {
      return Config::get("file");
    }
    if (Config::has("source")) {
      return std::string("temporaryModule");
    }
    return std::string("unreached");
  }();
  Generation::Generator visitor(kaubo::Object::PyString::Create(moduleName));

  visitor.visit(ast.unwrap());
  visitor.Visit();
  visitor.Emit();
  if (Config::has("show_ir")) {
    visitor.Print();
    IntermediateRepresentationTerminal::get_instance().terminate();
    ConsoleTerminal::get_instance().info("中间代码树生成完毕");
  }
  auto code = visitor.Code();
  if (Config::has("show_bc")) {
    VerboseTerminal::get_instance().switch_strategy(
      std::make_unique<ProxyTerminalStrategy>(&BytecodeTerminal::get_instance())
    );
    Object::PrintCode(code);
  }
  return code;
}

void interpret(const Object::PyCodePtr& code) {
  try {
    Runtime::VirtualMachine::Run(code);
  } catch (const std::exception& e) {
    // PrintFrame(kaubo::Runtime::VirtualMachine::Instance().CurrentFrame());
    ConsoleTerminal::get_instance().error(e.what());
    throw;
  }
}

}  // namespace kaubo