

#include "Generation/Generator.h"
#include <variant>
#include "Function/BuiltinFunction.h"
#include "IR/AssignStmt.h"
#include "IR/Expression/Atom.h"
#include "IR/Expression/Binary.h"
#include "IR/Expression/FunctionCall.h"
#include "IR/Expression/List.h"
#include "IR/Expression/Unary.h"
#include "IR/INode.h"
#include "IR/Identifier.h"
#include "IR/Lambda.h"
#include "IR/MemberAccess.h"
#include "IR/Module.h"
#include "IR/Statement/ExprStmt.h"
#include "IR/Statement/ForStmt.h"
#include "IR/Statement/IfStmt.h"
#include "IR/Statement/PassStmt.h"
#include "IR/Statement/ReturnStmt.h"
#include "Lexer/Type.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Parser/Expr.h"
#include "Parser/Module.h"
#include "Parser/Stmt.h"
#include "Utils/Overloaded.h"

namespace kaubo::Generation {

void Generator::visit(const Parser::ModulePtr& module) {
  auto stmts = module->statements;
  Collections::List<Object::PyObjPtr> statements(
    static_cast<uint64_t>(stmts.size())
  );
  for (const auto& stmt : stmts) {
    auto ir_stmt = this->visit_stmt(stmt);
    if (ir_stmt->is(Object::ListKlass::Self())) {
      throw std::runtime_error("Unexpected list");
    }
    statements.Push(ir_stmt->as<IR::INode>());
  }
  if (context->is(IR::ModuleKlass::Self())) {
    context->as<IR::Module>()->SetBody(Object::PyList::Create(statements));
    return;
  }
  if (context->is(IR::FuncDefKlass::Self())) {
    context->as<IR::FuncDef>()->SetBody(Object::PyList::Create(statements));
    return;
  }
  if (context->is(IR::LambdaKlass::Self())) {
    context->as<IR::Lambda>()->SetBody(Object::PyList::Create(statements));
    return;
  }
}

auto Generator::visit_stmt(const Parser::StmtPtr& stmt) -> Object::PyObjPtr {
  return std::visit(
    overloaded{
      [&](const std::shared_ptr<Parser::Stmt::Expr>& expr_stmt)
        -> Object::PyObjPtr {
        if (std::holds_alternative<std::shared_ptr<Parser::Expr::Binary>>(
              expr_stmt->expression->get_value()
            )) {
          auto binary_expr = std::get<std::shared_ptr<Parser::Expr::Binary>>(
            expr_stmt->expression->get_value()
          );
          if (binary_expr->op == Lexer::TokenType::Equal) {
            auto target = this->visit_expr(binary_expr->left);
            auto value = this->visit_expr(binary_expr->right);
            return IR::CreateAssignStmt(target, value, context);
          }
        }
        auto expr = this->visit_expr(expr_stmt->expression);
        return IR::CreateExprStmt(expr, context);
      },
      [&](const std::shared_ptr<Parser::Stmt::Empty>& /*empty_stmt*/)
        -> Object::PyObjPtr { return IR::CreatePassStmt(context); },
      [&](const std::shared_ptr<Parser::Stmt::Block>& block_stmt)
        -> Object::PyObjPtr {
        auto stmts = Object::PyList::Create();
        for (const auto& stmt : block_stmt->statements) {
          auto ir_stmt = this->visit_stmt(stmt);
          stmts->Append(ir_stmt);
        }
        return stmts;
      },
      [&](const std::shared_ptr<Parser::Stmt::VarDecl>& var_decl_stmt)
        -> Object::PyObjPtr {
        auto var_name = IR::CreateIdentifier(
          Object::PyString::Create(var_decl_stmt->name), context
        );
        auto var_value = this->visit_expr(var_decl_stmt->initializer);
        return IR::CreateAssignStmt(var_name, var_value, context);
      },
      [&](const std::shared_ptr<Parser::Stmt::If>& if_stmt)
        -> Object::PyObjPtr {
        auto condition = this->visit_expr(if_stmt->if_condition);
        auto thenStmtsResult = this->visit_stmt(if_stmt->then_body);
        if (!thenStmtsResult->is(Object::ListKlass::Self())) {
          throw std::runtime_error("Unexpected body");
        }
        auto thenStmts = thenStmtsResult->as<Object::PyList>();
        auto elseStmts = Object::PyList::Create();
        if (if_stmt->else_body != nullptr) {
          auto elseStmtsList = this->visit_stmt(if_stmt->else_body);
          if (!elseStmtsList->is(Object::ListKlass::Self())) {
            throw std::runtime_error("Unexpected body");
          }
          elseStmts = elseStmtsList->as<Object::PyList>();
        }
        auto elifs = Object::PyList::Create();
        auto elifConditions = Object::PyList::Create();
        for (size_t i = 0; i < if_stmt->elif_conditions.size(); ++i) {
          auto elifCondition = this->visit_expr(if_stmt->elif_conditions[i]);
          elifConditions->Append(elifCondition);
          auto elifStmtsList = this->visit_stmt(if_stmt->elif_bodies[i]);
          if (!elifStmtsList->is(Object::ListKlass::Self())) {
            throw std::runtime_error("Unexpected body");
          }
          auto elifStmts = elifStmtsList->as<Object::PyList>();
          elifs->Append(elifStmts);
        }
        return IR::CreateIfStmt(
          condition, thenStmts, elseStmts, elifs, elifConditions, context
        );
      },
      [&](const std::shared_ptr<Parser::Stmt::For>& for_stmt)
        -> Object::PyObjPtr {
        auto for_stmts_result = this->visit_stmt(for_stmt->body);
        if (!for_stmts_result->is(Object::ListKlass::Self())) {
          throw std::runtime_error("Unexpected body");
        }
        auto for_stmts = for_stmts_result->as<Object::PyList>();
        auto target = this->visit_expr(for_stmt->iterable);
        auto iter = this->visit_expr(for_stmt->iterator);
        return IR::CreateForStmt(target, iter, for_stmts, context);
      },
      [&](const std::shared_ptr<Parser::Stmt::While>& while_stmt)
        -> Object::PyObjPtr {
        auto while_stmts_result = this->visit_stmt(while_stmt->body);
        if (!while_stmts_result->is(Object::ListKlass::Self())) {
          throw std::runtime_error("Unexpected body");
        }
        auto while_stmts = while_stmts_result->as<Object::PyList>();
        auto condition = this->visit_expr(while_stmt->condition);
        return IR::CreateWhileStmt(condition, while_stmts, context);
      },
      [&](const std::shared_ptr<Parser::Stmt::Return>& return_stmt)
        -> Object::PyObjPtr {
        auto value = this->visit_expr(return_stmt->value);
        return IR::CreateReturnStmt(value, context);
      }
    },
    stmt->get_value()
  );
}

auto Generator::visit_expr(const Parser::ExprPtr& expr) -> IR::INodePtr {
  return std::visit(
    overloaded{
      [&](const std::shared_ptr<Parser::Expr::LiteralTrue>&) -> IR::INodePtr {
        return IR::CreateAtom(Object::PyBoolean::Create(true), context);
      },
      [&](const std::shared_ptr<Parser::Expr::LiteralFalse>&) -> IR::INodePtr {
        return IR::CreateAtom(Object::PyBoolean::Create(false), context);
      },
      [&](const std::shared_ptr<Parser::Expr::LiteralNull>&) -> IR::INodePtr {
        return IR::CreateAtom(Object::PyNone::Create(), context);
      },
      [&](const std::shared_ptr<Parser::Expr::LiteralList>& list_expr) {
        auto list = Object::PyList::Create();
        for (const auto& item : list_expr->elements) {
          list->Append(this->visit_expr(item));
        }
        return IR::CreateList(list, context);
      },
      [&](const std::shared_ptr<Parser::Expr::LiteralInt>& int_value_expr) {
        return IR::CreateAtom(
          Object::PyInteger::Create(int_value_expr->value), context
        );
      },
      [&](const std::shared_ptr<Parser::Expr::LiteralString>& str_value_expr)
        -> IR::INodePtr {
        return IR::CreateAtom(
          Object::PyString::Create(str_value_expr->value), context
        );
      },
      [&](
        const std::shared_ptr<Parser::Expr::MemberAccess>& member_access_expr
      ) {
        auto object = visit_expr(member_access_expr->object);
        auto member = Object::PyString::Create(member_access_expr->member);
        return IR::CreateMemberAccess(object, member, context);
      },
      [&](const std::shared_ptr<Parser::Expr::Lambda>& lambda_expr)
        -> IR::INodePtr {
        auto parameters = Object::PyList::Create();
        for (const auto& param : lambda_expr->params) {
          parameters->Append(Object::PyString::Create(param));
        }
        auto lambda =
          IR::CreateLambda(parameters, Object::PyList::Create(), context)
            ->as<IR::Lambda>();
        auto oldContext = context;
        context = lambda;
        auto body = this->visit_stmt(lambda_expr->body)->as<Object::PyList>();
        if (body == nullptr) {
          throw std::runtime_error("Unexpected body");
        }
        lambda->SetBody(body);
        context = oldContext;
        return lambda;
      },
      [&](const std::shared_ptr<Parser::Expr::Binary>& binary_expr)
        -> IR::INodePtr {
        auto left = this->visit_expr(binary_expr->left);
        auto right = this->visit_expr(binary_expr->right);
        auto oprt = IR::Binary::Operator::AND;
        switch (binary_expr->op) {
          case Lexer::TokenType::Plus:
            oprt = IR::Binary::Operator::ADD;
            break;
          case Lexer::TokenType::Minus:
            oprt = IR::Binary::Operator::SUB;
            break;
          case Lexer::TokenType::Asterisk:
            oprt = IR::Binary::Operator::MUL;
            break;
          case Lexer::TokenType::Slash:
            oprt = IR::Binary::Operator::FLOOR_DIV;
            break;
          case Lexer::TokenType::DoubleEqual:
            oprt = IR::Binary::Operator::EQ;
            break;
          case Lexer::TokenType::ExclamationEqual:
            oprt = IR::Binary::Operator::NE;
            break;
          case Lexer::TokenType::GreaterThan:
            oprt = IR::Binary::Operator::GT;
            break;
          case Lexer::TokenType::GreaterThanEqual:
            oprt = IR::Binary::Operator::GE;
            break;
          case Lexer::TokenType::LessThan:
            oprt = IR::Binary::Operator::LT;
            break;
          case Lexer::TokenType::LessThanEqual:
            oprt = IR::Binary::Operator::LE;
            break;
          case Lexer::TokenType::And:
            oprt = IR::Binary::Operator::AND;
            break;
          case Lexer::TokenType::Or:
            oprt = IR::Binary::Operator::OR;
            break;
          case Lexer::TokenType::Percent:
            oprt = IR::Binary::Operator::MOD;
            break;
          default:
            break;
        }
        return IR::CreateBinary(oprt, left, right, context);
      },
      [&](const std::shared_ptr<Parser::Expr::Unary>& unary) -> IR::INodePtr {
        auto oprt = IR::Unary::Operator::INVERT;
        switch (unary->op) {
          case Lexer::TokenType::Minus:
            oprt = IR::Unary::Operator::MINUS;
            break;
          case Lexer::TokenType::Plus:
            oprt = IR::Unary::Operator::PLUS;
            break;
          case Lexer::TokenType::Not:
            oprt = IR::Unary::Operator::NOT;
          default:
            break;
        }
        auto expr = this->visit_expr(unary->operand);
        return IR::CreateUnary(oprt, expr, context);
      },
      [&](const std::shared_ptr<Parser::Expr::VarRef>& var_ref_expr)
        -> IR::INodePtr {
        return IR::CreateIdentifier(
          Object::PyString::Create(var_ref_expr->name), context
        );
      },
      [&](const std::shared_ptr<Parser::Expr::FunctionCall>& function_call_expr)
        -> IR::INodePtr {
        auto func = this->visit_expr(function_call_expr->function_expr);
        auto args = Object::PyList::Create();
        for (const auto& arg : function_call_expr->arguments) {
          args->Append(this->visit_expr(arg));
        }
        return IR::CreateFunctionCall(func, args, context);
      },
      [&](const std::shared_ptr<Parser::Expr::Grouping>& grouping_expr)
        -> IR::INodePtr { return this->visit_expr(grouping_expr->expression); },
      [&](const std::shared_ptr<Parser::Expr::Assign>& var_assign_expr)
        -> IR::INodePtr {
        auto var_name = IR::CreateIdentifier(
          Object::PyString::Create(var_assign_expr->name), context
        );
        auto var_value = this->visit_expr(var_assign_expr->value);
        return IR::CreateAssignStmt(var_name, var_value, context);
      }
    }  // namespace kaubo::Generation
    ,
    expr->get_value()
  );
}
}  // namespace kaubo::Generation