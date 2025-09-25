#pragma once

#include "Common.h"
#include "Lexer/Core/Proto.h"
#include "Lexer/Type.h"
#include "Parser/Error.h"
#include "Parser/Listener.h"
#include "Parser/Module.h"
#include "Utils/Result.h"

namespace kaubo::Parser {
using Lexer::TokenType;
using Utils::Err;
using Utils::Ok;
using Utils::Result;

// Pratt parser实现
class Parser {
 public:
  explicit Parser(Lexer::Instance<TokenType> lexer)
    : m_lexer(std::move(lexer)) {
    consume();  // 预读第一个token
  }

  auto parse() -> Result<ModulePtr, Error>;

  auto bind_listener(const ListenerPtr& listener) -> void {
    listeners.push_back(listener);
  }

 private:
  Lexer::Instance<TokenType> m_lexer;
  std::optional<Lexer::Token::Proto<TokenType>> current_token;

  // 消费当前token并读取下一个
  void consume() { current_token = m_lexer->next_token(); }

  // 检查当前token是否为指定类型
  [[nodiscard]] auto check(TokenType type) const -> bool {
    return current_token.has_value() && current_token->type == type;
  }

  // 检查并消费指定类型的token
  auto match(TokenType type) -> bool {
    if (check(type)) {
      consume();
      return true;
    }
    return false;
  }

  // 期望并消费指定类型的token，否则返回错误
  auto expect(TokenType type) -> Result<void, Error> {
    if (check(type)) {
      consume();
      return Ok();
    }
    return Err(Error::UnexpectedToken);
  }

  auto parse_expression(int32_t precedence = 0) -> Result<ExprPtr, Error>;
  auto parse_primary() -> Result<ExprPtr, Error>;
  auto parse_primary_base() -> Result<ExprPtr, Error>;
  auto parse_int() -> Result<ExprPtr, Error>;
  auto parse_string() -> Result<ExprPtr, Error>;
  auto parse_list() -> Result<ExprPtr, Error>;
  auto parse_identifier_expression() -> Result<ExprPtr, Error>;
  auto parse_unary() -> Result<ExprPtr, Error>;
  auto parse_parenthesized() -> Result<ExprPtr, Error>;
  auto parse_lambda() -> Result<ExprPtr, Error>;
  auto parse_function_call(ExprPtr) -> Result<ExprPtr, Error>;
  auto parse_postfix(ExprPtr expr) -> Result<ExprPtr, Error>;

  auto parse_statement() -> Result<StmtPtr, Error>;
  auto parse_block() -> Result<StmtPtr, Error>;
  auto parse_var_declaration() -> Result<StmtPtr, Error>;
  auto parse_return_statement() -> Result<StmtPtr, Error>;
  auto parse_for_loop() -> Result<StmtPtr, Error>;
  auto parse_while_loop() -> Result<StmtPtr, Error>;
  auto parse_if_statement() -> Result<StmtPtr, Error>;
  auto parse_module() -> Result<ModulePtr, Error>;

  std::vector<ListenerPtr> listeners;

  auto enter_module() -> void {
    for (const auto& listener : listeners) {
      listener->on_enter_module();
    }
  }
  auto exit_module(const ModulePtr& module) -> void {
    for (const auto& listener : listeners) {
      listener->on_exit_module(module);
    }
  }
  auto enter_statement() -> void {
    for (const auto& listener : listeners) {
      listener->on_enter_statement();
    }
  }
  auto exit_statement(const StmtPtr& stmt) -> void {
    for (const auto& listener : listeners) {
      listener->on_exit_statement(stmt);
    }
  }
  auto enter_expr() -> void {
    for (const auto& listener : listeners) {
      listener->on_enter_expr();
    }
  }
  auto exit_expr(const ExprPtr& expr) -> void {
    for (const auto& listener : listeners) {
      listener->on_exit_expr(expr);
    }
  }
};

}  // namespace kaubo::Parser
