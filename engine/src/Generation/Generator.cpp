

#include "Generation/Generator.h"
#include "IR/AssignStmt.h"
#include "IR/Expression/Atom.h"
#include "IR/Expression/Binary.h"
#include "IR/Expression/FunctionCall.h"
#include "IR/INode.h"
#include "IR/Identifier.h"
#include "IR/Module.h"
#include "IR/Statement/ExprStmt.h"
#include "IR/Statement/PassStmt.h"
#include "Lexer/Type.h"
#include "Object/Container/PyList.h"
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
    statements.Push(ir_stmt);
  }
  if (context->is(IR::ModuleKlass::Self())) {
    context->as<IR::Module>()->SetBody(Object::PyList::Create(statements));
    return;
  }
  if (context->is(IR::FuncDefKlass::Self())) {
    context->as<IR::FuncDef>()->SetBody(Object::PyList::Create(statements));
    return;
  }
}

IR::INodePtr Generator::visit_stmt(const Parser::StmtPtr& stmt) {
  return std::visit(
    overloaded{
      [&](const std::shared_ptr<Parser::Stmt::Expr>& expr_stmt)
        -> IR::INodePtr {
        auto expr = this->visit_expr(expr_stmt->expression);
        return IR::CreateExprStmt(expr, context);
      },
      [&](const std::shared_ptr<Parser::Stmt::Empty>& /*empty_stmt*/)
        -> IR::INodePtr { return IR::CreatePassStmt(context); },
      [&](const std::shared_ptr<Parser::Stmt::Block>&) -> IR::INodePtr {
        return nullptr;
      },
      [&](const std::shared_ptr<Parser::Stmt::VarDecl>& var_decl_stmt)
        -> IR::INodePtr {
        auto var_name = IR::CreateIdentifier(
          Object::PyString::Create(var_decl_stmt->name), context
        );
        auto var_value = this->visit_expr(var_decl_stmt->initializer);
        return IR::CreateAssignStmt(var_name, var_value, context);
      }
    },
    stmt->get_value()
  );
}

IR::INodePtr Generator::visit_expr(const Parser::ExprPtr& expr) {
  return std::visit(
    overloaded{
      [&](Parser::Expr::IntValue int_value_expr) -> IR::INodePtr {
        return IR::CreateAtom(
          Object::PyInteger::Create(int_value_expr), context
        );
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
          case Lexer::TokenType::Multiply:
            oprt = IR::Binary::Operator::MUL;
            break;
          case Lexer::TokenType::Divide:
            oprt = IR::Binary::Operator::DIV;
            break;
          default:
            break;
        }
        return IR::CreateBinary(oprt, left, right, context);
      },
      [&](const std::shared_ptr<Parser::Expr::Unary>&) -> IR::INodePtr {
        return nullptr;
      },
      [&](const std::shared_ptr<Parser::Expr::VarRef>& var_ref_expr)
        -> IR::INodePtr {
        return IR::CreateIdentifier(
          Object::PyString::Create(var_ref_expr->name), context
        );
      },
      [&](const std::shared_ptr<Parser::Expr::FunctionCall>& function_call_expr)
        -> IR::INodePtr {
        auto func_name = IR::CreateIdentifier(
          Object::PyString::Create(function_call_expr->function_name), context
        );
        auto args = Object::PyList::Create();
        for (const auto& arg : function_call_expr->arguments) {
          args->Append(this->visit_expr(arg));
        }
        return IR::CreateFunctionCall(func_name, args, context);
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
    },
    expr->get_value()
  );
}
}  // namespace kaubo::Generation