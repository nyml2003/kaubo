#pragma once

#include "IR/FuncDef.h"
#include "IR/INode.h"

#include "IR/Module.h"
#include "IR/Statement/WhileStmt.h"
#include "Object/Container/PyList.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Parser/Expr.h"
#include "Parser/Module.h"
#include "Parser/Stmt.h"

#include <fstream>
#include <sstream>
#include <string>

namespace kaubo::Generation {

inline auto read_file(const std::string& filename) -> std::string {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("无法打开文件: " + filename);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

class Generator {
 private:
  Object::PyListPtr codeList;
  IR::INodePtr context;

 public:
  explicit Generator(const Object::PyStrPtr& filename)
    : codeList(Object::PyList::Create()) {
    context = IR::CreateModule(Object::PyList::Create(), filename);
  }

  void Visit() { context->visit(codeList); }
  void Emit() { context->emit(codeList); }
  void Print() { context->print(); }
  [[nodiscard]] Object::PyCodePtr Code() const {
    return IR::GetCodeFromList(codeList, context);
  }

  void visit(const Parser::ModulePtr& module);

  IR::INodePtr visit_stmt(const Parser::StmtPtr& stmt);

  IR::INodePtr visit_expr(const Parser::ExprPtr& expr);
};
}  // namespace kaubo::Generation