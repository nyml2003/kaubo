#include "Interface.h"
#include "Binding/Interface/impl.h"
#include "Collections/String/BytesHelper.h"
#include "Generation/Generator.h"
#include "IR/IRHelper.h"
#include "Object/Core/CoreHelper.h"
#include "Python3Lexer.h"
#include "Python3Parser.h"
#include "Runtime/BinaryFileParser.h"
#include "Runtime/VirtualMachine.h"
#include "Tools/Config/Config.h"
#include "Tools/Terminal/IntermediateRepresentationTerminal.h"
#include "Tools/Terminal/Terminal.h"
#include "Tools/Terminal/VerboseTerminal.h"

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
  auto code = compile(CreateANTLRInputStream());
  if (Config::has("file")) {
    auto data = code->_serialize_()->as<kaubo::Object::PyBytes>();
    const auto& bytes = data->Value();
    auto filePath = Config::get("file");
    auto writePath = std::filesystem::path(filePath).replace_extension(".code");
    Collections::Write(bytes, writePath.string());
  }
}

void handle_mode_interpret() {
  auto code = compile(CreateANTLRInputStream());
  interpret(code);
}

void handle_mode_interpret_bytecode() {
  auto filename = Config::get("file");
  kaubo::Runtime::BinaryFileParser parser(filename);
  auto code = parser.Parse();
  interpret(code);
}

InputStreamPtr CreateANTLRInputStream() {
  if (Config::has("file")) {
    ConsoleTerminal::get_instance().debug("文件名: " + Config::get("file"));
    std::ifstream file(Config::get("file"));
    return std::make_unique<antlr4::ANTLRInputStream>(file);
  }
  if (Config::has("source")) {
    return std::make_unique<antlr4::ANTLRInputStream>(Config::get("source"));
  }
  throw std::runtime_error("未指定文件或源码");
}

Object::PyCodePtr compile(InputStreamPtr input_stream) {
  Python3Lexer lexer(input_stream.get());
  antlr4::CommonTokenStream tokens(&lexer);
  Python3Parser parser(&tokens);

  antlr4::tree::ParseTree* tree = parser.file_input();

  //  // 打印词法
  if (Config::has("show_tokens")) {
    for (const auto& token : tokens.getTokens()) {
      LexicalAnalysisTerminal::get_instance().info(token->toString());
    }
    ConsoleTerminal::get_instance().info("词法单元流生成完毕");
  }
  if (Config::has("show_ast")) {
    SyntaxAnalysisTerminal::get_instance().info(tree->toStringTree(&parser));
    ConsoleTerminal::get_instance().info("抽象语法树生成完毕");
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

  visitor.visit(tree);
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