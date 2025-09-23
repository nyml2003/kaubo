#pragma once
#include "Common.h"
#include "Lexer/Type.h"

#include <memory>
#include <variant>
#include <vector>

namespace kaubo::Parser::Expr {
using Lexer::TokenType;

struct LiteralInt {
  int64_t value;
};

struct LiteralString {
  std::string value;
};

// 二元运算符表达式
struct Binary {
  ExprPtr left;
  TokenType op;
  ExprPtr right;
};

// 一元运算符表达式
struct Unary {
  TokenType op;
  ExprPtr operand;
};

// 括号表达式
struct Grouping {
  ExprPtr expression;
};
// 变量引用表达式
struct VarRef {
  std::string name;
};

// 函数调用表达式
struct FunctionCall {
  std::string function_name;
  std::vector<ExprPtr> arguments;
};

// 赋值表达式
struct Assign {
  std::string name;
  ExprPtr value;
};

// Lambda表达式
struct Lambda {
  std::vector<std::string> params;
  Parser::StmtPtr body;
};

class Expr {
 public:
  using ValueType = std::variant<
    std::shared_ptr<LiteralString>,
    std::shared_ptr<LiteralInt>,
    std::shared_ptr<Binary>,
    std::shared_ptr<Unary>,
    std::shared_ptr<Grouping>,
    std::shared_ptr<VarRef>,
    std::shared_ptr<FunctionCall>,
    std::shared_ptr<Assign>,
    std::shared_ptr<Lambda>>;

  explicit Expr() = delete;

  template <typename T>
  explicit Expr(std::shared_ptr<T> expr) : m_value(std::move(expr)) {}

  // 获取值类型的访问方法
  [[nodiscard]] auto get_value() const -> const ValueType& { return m_value; }

 private:
  ValueType m_value;
};

}  // namespace kaubo::Parser::Expr