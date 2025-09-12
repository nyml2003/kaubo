#include "Generation/Generator.h"
#include "IR/AssignStmt.h"
#include "IR/ClassDef.h"
#include "IR/Expression/Atom.h"
#include "IR/Expression/Binary.h"
#include "IR/Expression/FunctionCall.h"
#include "IR/Expression/List.h"
#include "IR/Expression/Map.h"
#include "IR/Expression/Slice.h"
#include "IR/Expression/Unary.h"
#include "IR/Expression/YieldExpr.h"
#include "IR/FuncDef.h"
#include "IR/INode.h"
#include "IR/Identifier.h"
#include "IR/MemberAccess.h"
#include "IR/Module.h"
#include "IR/Statement/ExprStmt.h"
#include "IR/Statement/ForStmt.h"
#include "IR/Statement/IfStmt.h"
#include "IR/Statement/PassStmt.h"
#include "IR/Statement/ReturnStmt.h"
#include "IR/Statement/WhileStmt.h"
#include "Object/Container/PyList.h"
#include "Object/Core/PyBoolean.h"
#include "Object/Core/PyNone.h"
#include "Object/Number/PyFloat.h"
#include "Object/Number/PyInteger.h"
#include "Object/Object.h"
#include "Object/String/PyString.h"
#include "Tools/Terminal/Terminal.h"

#include "support/Any.h"

namespace kaubo::Generation {

Generator::Generator(const Object::PyStrPtr& filename)
  : codeList(Object::CreatePyList()) {
  context = IR::CreateModule(Object::CreatePyList(), filename);
}

void Generator::Visit() {
  context->visit(codeList);
}

void Generator::Emit() {
  context->emit(codeList);
}

void Generator::Print() {
  context->print();
}

[[nodiscard]] Object::PyCodePtr Generator::Code() const {
  return IR::GetCodeFromList(codeList, context);
}

antlrcpp::Any Generator::visitStmt(Python3Parser::StmtContext* ctx) {
  if (ctx->simple_stmts() != nullptr) {
    return visitSimple_stmts(ctx->simple_stmts());
  }
  if (ctx->compound_stmt() != nullptr) {
    return visitCompound_stmt(ctx->compound_stmt());
  }
  return nullptr;
}

antlrcpp::Any Generator::visitFile_input(
  Python3Parser::File_inputContext* ctx
) {
  auto stmts = ctx->stmt();
  Collections::List<Object::PyObjPtr> statements(
    static_cast<uint64_t>(stmts.size())
  );
  for (auto* stmtItertor : stmts) {
    auto stmt = visitStmt(stmtItertor);
    if (stmt.type() == typeid(IR::INodePtr)) {
      statements.Push(std::any_cast<IR::INodePtr>(stmt));
      continue;
    }
    if (stmt.type() == typeid(Object::PyListPtr)) {
      auto list = std::any_cast<Object::PyListPtr>(stmt);
      for (Index i = 0; i < list->Length(); i++) {
        statements.Push(list->GetItem(i));
      }
      continue;
    }
    ConsoleTerminal::get_instance().error(
      "visitFile_input: Unknown statement type"
    );
  }
  if (context->is(IR::ModuleKlass::Self())) {
    context->as<IR::Module>()->SetBody(Object::CreatePyList(statements));
    return nullptr;
  }
  if (context->is(IR::FuncDefKlass::Self())) {
    context->as<IR::FuncDef>()->SetBody(Object::CreatePyList(statements));
    return nullptr;
  }
  ConsoleTerminal::get_instance().error(
    "visitFile_input: Unknown context type"
  );
  return nullptr;
}

antlrcpp::Any Generator::visitTestlist_comp(
  Python3Parser::Testlist_compContext* ctx
) {
  auto testlist = ctx->test();
  Collections::List<Object::PyObjPtr> tests(
    static_cast<uint64_t>(testlist.size())
  );
  for (auto* test : testlist) {
    tests.Push(std::any_cast<IR::INodePtr>(visitTest(test)));
  }
  return Object::CreatePyList(tests);
}

antlrcpp::Any Generator::visitAtom(Python3Parser::AtomContext* ctx) {
  if (ctx->OPEN_PAREN() != nullptr) {
    if (ctx->testlist_comp() != nullptr) {
      return visitTest(ctx->testlist_comp()->test(0));
    }
    if (ctx->yield_expr() != nullptr) {
      return visitYield_expr(ctx->yield_expr());
    }
  }
  if (ctx->OPEN_BRACK() != nullptr) {
    // 情况 2: '[' testlist_comp? ']'
    if (ctx->testlist_comp() == nullptr) {
      return IR::CreateList(Object::CreatePyList(), context);
    }
    auto testlist_comp =
      std::any_cast<Object::PyListPtr>(visitTestlist_comp(ctx->testlist_comp())
      );
    return IR::CreateList(testlist_comp, context);
  }
  if (ctx->OPEN_BRACE() != nullptr) {
    auto dictorsetmaker = ctx->dictorsetmaker();
    if (dictorsetmaker == nullptr) {
      return IR::CreateMap(
        Object::CreatePyList(), Object::CreatePyList(), context
      );
    }
    auto dictorset = ctx->dictorsetmaker();
    auto tests = dictorset->test();
    Collections::List<Object::PyObjPtr> keys(
      static_cast<uint64_t>(tests.size() / 2)
    );
    Collections::List<Object::PyObjPtr> values(
      static_cast<uint64_t>(tests.size()) / 2
    );
    for (Index i = 0; i < tests.size(); i += 2) {
      keys.Push(std::any_cast<IR::INodePtr>(visitTest(tests[i])));
      values.Push(std::any_cast<IR::INodePtr>(visitTest(tests[i + 1])));
    }
    return IR::CreateMap(
      Object::CreatePyList(keys), Object::CreatePyList(values), context
    );
  }
  if (ctx->name() != nullptr) {
    // 情况 4: name
    return IR::CreateIdentifier(
      Object::PyString::Create(ctx->name()->getText().c_str()), context
    );
    // visitName(ctx->name());
  }
  if (ctx->NUMBER() != nullptr) {
    // 情况 5: NUMBER
    std::string numberText = ctx->NUMBER()->getText();

    // 检查字符串中是否包含小数点
    if (numberText.find('.') != std::string::npos) {
      // 情况 5.1: Float
      return IR::CreateAtom(
        Object::CreatePyFloat(std::stod(numberText)), context
      );
    }
    // 情况 5.2: Integer
    return IR::CreateAtom(
      Object::CreatePyInteger(
        Collections::CreateIntegerWithCString(numberText.c_str())
      ),
      context
    );
  }
  if (!ctx->STRING().empty()) {
    // 情况 6: STRING+
    Object::PyObjPtr str = Object::PyString::Create("");
    for (auto* string : ctx->STRING()) {
      auto rawString = string->getText();
      rawString = rawString.substr(1, rawString.size() - 2);
      str = str->add(Object::PyString::Create(rawString));
    }
    return IR::CreateAtom(str, context);
  }
  if (ctx->ELLIPSIS() != nullptr) {
    // 情况 7: '...'
    std::cout << "atom: '...'" << std::endl;

  } else if (ctx->NONE() != nullptr) {
    // 情况 8: 'None'
    return IR::CreateAtom(Object::CreatePyNone(), context);
  } else if (ctx->TRUE() != nullptr) {
    // 情况 9: 'True'
    return IR::CreateAtom(Object::PyBoolean::Create(true), context);
  } else if (ctx->FALSE() != nullptr) {
    // 情况 10: 'False'
    return IR::CreateAtom(Object::PyBoolean::Create(false), context);
  } else {
    // 其他情况
    std::cout << "atom: Unknown type" << std::endl;
  }
  return nullptr;
}

antlrcpp::Any Generator::visitExpr(Python3Parser::ExprContext* ctx) {
  if (ctx->atom_expr() != nullptr) {
    return visitAtom_expr(ctx->atom_expr());
  }
  auto exprs = ctx->expr();
  if (exprs.size() == 1) {
    auto operand = std::any_cast<IR::INodePtr>(visitExpr(exprs[0]));
    if (ctx->ADD().size() == 1) {
      return IR::CreateUnary(IR::Unary::Operator::PLUS, operand, context);
    }
    if (ctx->MINUS().size() == 1) {
      return IR::CreateUnary(IR::Unary::Operator::MINUS, operand, context);
    }
    if (ctx->NOT_OP().size() == 1) {
      return IR::CreateUnary(IR::Unary::Operator::INVERT, operand, context);
    }
  }
  if (exprs.size() == 2) {
    auto left = std::any_cast<IR::INodePtr>(visitExpr(exprs[0]));
    auto right = std::any_cast<IR::INodePtr>(visitExpr(exprs[1]));
    if (ctx->STAR() != nullptr) {
      return IR::CreateBinary(IR::Binary::Operator::MUL, left, right, context);
    }
    if (ctx->POWER() != nullptr) {
      return IR::CreateBinary(
        IR::Binary::Operator::POWER, left, right, context
      );
    }
    if (ctx->AT() != nullptr) {
      return IR::CreateBinary(
        IR::Binary::Operator::MATMUL, left, right, context
      );
    }
    if (ctx->DIV() != nullptr) {
      return IR::CreateBinary(IR::Binary::Operator::DIV, left, right, context);
    }
    if (ctx->MOD() != nullptr) {
      return IR::CreateBinary(IR::Binary::Operator::MOD, left, right, context);
    }
    if (ctx->IDIV() != nullptr) {
      return IR::CreateBinary(
        IR::Binary::Operator::FLOOR_DIV, left, right, context
      );
    }
    if (ctx->ADD().size() == 1) {
      return IR::CreateBinary(IR::Binary::Operator::ADD, left, right, context);
    }
    if (ctx->MINUS().size() == 1) {
      return IR::CreateBinary(IR::Binary::Operator::SUB, left, right, context);
    }
    if (ctx->LEFT_SHIFT() != nullptr) {
      return IR::CreateBinary(
        IR::Binary::Operator::LSHIFT, left, right, context
      );
    }
    if (ctx->RIGHT_SHIFT() != nullptr) {
      return IR::CreateBinary(
        IR::Binary::Operator::RSHIFT, left, right, context
      );
    }
    if (ctx->AND_OP() != nullptr) {
      return IR::CreateBinary(IR::Binary::Operator::AND, left, right, context);
    }
    if (ctx->XOR() != nullptr) {
      return IR::CreateBinary(IR::Binary::Operator::XOR, left, right, context);
    }
    if (ctx->OR_OP() != nullptr) {
      return IR::CreateBinary(IR::Binary::Operator::OR, left, right, context);
    }
  }
  throw std::runtime_error("Unknown expression type");
}

antlrcpp::Any Generator::visitExpr_stmt(Python3Parser::Expr_stmtContext* ctx) {
  // 处理类型注解赋值 (annassign)
  if (ctx->annassign() != nullptr) {
    std::cout << "Annotation assignment" << std::endl;
    // return visitAnnassign(ctx->annassign());
    return nullptr;
  }

  // 处理增强赋值 (augassign)
  if (ctx->augassign() != nullptr) {
    auto target = std::any_cast<IR::INodePtr>(
      visitTestlist_star_expr(ctx->testlist_star_expr(0))
    );
    auto targetCopy = std::any_cast<IR::INodePtr>(
      visitTestlist_star_expr(ctx->testlist_star_expr(0))
    );
    auto source = std::any_cast<IR::INodePtr>(visitTestlist(ctx->testlist()));
    if (ctx->augassign()->ADD_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::ADD, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->SUB_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::SUB, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->MULT_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::MUL, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->AT_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::MATMUL, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->DIV_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::DIV, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->MOD_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::MOD, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->AND_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::AND, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->OR_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(IR::Binary::Operator::OR, targetCopy, source, context),
        context
      );
    }
    if (ctx->augassign()->XOR_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::XOR, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->LEFT_SHIFT_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::LSHIFT, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->RIGHT_SHIFT_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::RSHIFT, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->POWER_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::POWER, targetCopy, source, context
        ),
        context
      );
    }
    if (ctx->augassign()->IDIV_ASSIGN() != nullptr) {
      return IR::CreateAssignStmt(
        target,
        IR::CreateBinary(
          IR::Binary::Operator::FLOOR_DIV, targetCopy, source, context
        ),
        context
      );
    }
  }

  // 处理普通赋值或多重赋值
  if (!ctx->ASSIGN().empty()) {
    IR::INodePtr source = nullptr;
    if (ctx->testlist_star_expr().size() == 2) {
      source = std::any_cast<IR::INodePtr>(
        visitTestlist_star_expr(ctx->testlist_star_expr(1))
      );
    } else if (ctx->testlist_star_expr().size() == 1) {
      source = std::any_cast<IR::INodePtr>(visitYield_expr(ctx->yield_expr(0)));
    }

    auto target = std::any_cast<IR::INodePtr>(
      visitTestlist_star_expr(ctx->testlist_star_expr(0))
    );
    return IR::CreateAssignStmt(target, source, context);
  }

  // 处理只有 testlist_star_expr 的情况
  if (ctx->ASSIGN().empty()) {
    return IR::CreateExprStmt(
      std::any_cast<IR::INodePtr>(
        visitTestlist_star_expr(ctx->testlist_star_expr(0))
      ),
      context
    );
  }

  return nullptr;
}

antlrcpp::Any Generator::visitArglist(Python3Parser::ArglistContext* ctx) {
  if (ctx->argument().empty()) {
    return Object::CreatePyList();
  }
  auto arglist = ctx->argument();
  Collections::List<Object::PyObjPtr> args(
    static_cast<uint64_t>(arglist.size())
  );
  for (auto* arg : arglist) {
    args.Push(std::any_cast<IR::INodePtr>(visitArgument(arg)));
  }
  return Object::CreatePyList(args);
}

antlrcpp::Any Generator::visitArgument(Python3Parser::ArgumentContext* ctx) {
  return visitTest(ctx->test(0));
}

antlrcpp::Any Generator::visitAtom_expr(Python3Parser::Atom_exprContext* ctx) {
  if (ctx->trailer().empty()) {
    if (ctx->atom() != nullptr) {
      return std::any_cast<IR::INodePtr>(visitAtom(ctx->atom()));
    }
    return nullptr;
  }
  auto identifier = std::any_cast<IR::INodePtr>(visitAtom(ctx->atom()));
  auto result = identifier;
  auto trailers = ctx->trailer();
  for (auto& trailer : trailers) {
    if (trailer->OPEN_PAREN() != nullptr) {  // '('
      if (trailer->arglist() == nullptr) {
        result =
          IR::CreateFunctionCall(result, Object::CreatePyList(), context);
      } else {
        auto args =
          std::any_cast<Object::PyListPtr>(visitArglist(trailer->arglist()));
        result = IR::CreateFunctionCall(result, args, context);
      }
    } else if (trailer->OPEN_BRACK() != nullptr) {  // '['
      auto args =
        std::any_cast<IR::INodePtr>(visitSubscriptlist(trailer->subscriptlist())
        );
      result =
        IR::CreateBinary(IR::Binary::Operator::SUBSCR, result, args, context);
    } else if (trailer->DOT() != nullptr) {  // '.'
      auto args = Object::PyString::Create(trailer->name()->getText().c_str());
      result = IR::CreateMemberAccess(result, args, context);
    }
  }
  return result;
}

antlrcpp::Any Generator::visitTestlist_star_expr(
  Python3Parser::Testlist_star_exprContext* ctx
) {
  return visitTest(ctx->test(0));
}

antlrcpp::Any Generator::visitTest(Python3Parser::TestContext* ctx) {
  return visitOr_test(ctx->or_test(0));
}

antlrcpp::Any Generator::visitOr_test(Python3Parser::Or_testContext* ctx) {
  auto andTests = ctx->and_test();
  auto left = std::any_cast<IR::INodePtr>(visitAnd_test(andTests[0]));
  if (andTests.size() == 1) {
    return left;
  }
  IR::INodePtr right = nullptr;
  for (Index i = 1; i < andTests.size(); ++i) {
    right = std::any_cast<IR::INodePtr>(visitAnd_test(andTests[i]));
    left = IR::CreateBinary(IR::Binary::Operator::OR, left, right, context);
  }
  return left;
}

antlrcpp::Any Generator::visitAnd_test(Python3Parser::And_testContext* ctx) {
  auto notTests = ctx->not_test();
  auto left = std::any_cast<IR::INodePtr>(visitNot_test(notTests[0]));
  if (notTests.size() == 1) {
    return left;
  }
  IR::INodePtr right = nullptr;
  for (Index i = 1; i < notTests.size(); ++i) {
    right = std::any_cast<IR::INodePtr>(visitNot_test(notTests[i]));
    left = IR::CreateBinary(IR::Binary::Operator::AND, left, right, context);
  }
  return left;
}

antlrcpp::Any Generator::visitNot_test(Python3Parser::Not_testContext* ctx) {
  if (ctx->comparison() != nullptr) {
    return visitComparison(ctx->comparison());
  }
  if (ctx->NOT() != nullptr) {
    auto operand = std::any_cast<IR::INodePtr>(visitNot_test(ctx->not_test()));
    return IR::CreateUnary(IR::Unary::Operator::NOT, operand, context);
  }
  throw std::runtime_error("visitNot_test: Unknown type");
}

antlrcpp::Any Generator::visitComparison(
  Python3Parser::ComparisonContext* ctx
) {
  if (ctx->comp_op().empty()) {
    return visitExpr(ctx->expr(0));
  }
  auto left = std::any_cast<IR::INodePtr>(visitExpr(ctx->expr(0)));
  auto right = std::any_cast<IR::INodePtr>(visitExpr(ctx->expr(1)));
  if (ctx->comp_op(0)->GT_EQ() != nullptr) {
    return CreateBinary(IR::Binary::Operator::GE, left, right, context);
  }
  if (ctx->comp_op(0)->LT_EQ() != nullptr) {
    return CreateBinary(IR::Binary::Operator::LE, left, right, context);
  }
  if (ctx->comp_op(0)->EQUALS() != nullptr) {
    return CreateBinary(IR::Binary::Operator::EQ, left, right, context);
  }
  if ((ctx->comp_op(0)->NOT_EQ_1() != nullptr) ||
      (ctx->comp_op(0)->NOT_EQ_2() != nullptr)) {
    return CreateBinary(IR::Binary::Operator::NE, left, right, context);
  }
  if (ctx->comp_op(0)->LESS_THAN() != nullptr) {
    return CreateBinary(IR::Binary::Operator::LT, left, right, context);
  }
  if (ctx->comp_op(0)->GREATER_THAN() != nullptr) {
    return CreateBinary(IR::Binary::Operator::GT, left, right, context);
  }
  if (ctx->comp_op(0)->getText() == "in") {
    return CreateBinary(IR::Binary::Operator::IN_OP, left, right, context);
  }
  if (ctx->comp_op(0)->getText() == "notin") {
    return CreateBinary(IR::Binary::Operator::NOT_IN, left, right, context);
  }
  if (ctx->comp_op(0)->getText() == "is") {
    return CreateBinary(IR::Binary::Operator::IS, left, right, context);
  }
  if (ctx->comp_op(0)->getText() == "isnot") {
    return CreateBinary(IR::Binary::Operator::IS_NOT, left, right, context);
  }
  return nullptr;
}

antlrcpp::Any Generator::visitCompound_stmt(
  Python3Parser::Compound_stmtContext* ctx
) {
  // 1. 如果是 if_stmt
  if (ctx->if_stmt() != nullptr) {
    return visitIf_stmt(ctx->if_stmt());
  }
  // 2. 如果是 while_stmt
  if (ctx->while_stmt() != nullptr) {
    return visitWhile_stmt(ctx->while_stmt());
  }
  // 3. 如果是 for_stmt
  if (ctx->for_stmt() != nullptr) {
    return visitFor_stmt(ctx->for_stmt());
  }
  // 4. 如果是 try_stmt
  if (ctx->try_stmt() != nullptr) {
    return visitTry_stmt(ctx->try_stmt());
  }
  // 5. 如果是 with_stmt
  if (ctx->with_stmt() != nullptr) {
    return visitWith_stmt(ctx->with_stmt());
  }
  // 6. 如果是 funcdef
  if (ctx->funcdef() != nullptr) {
    return visitFuncdef(ctx->funcdef());
  }
  // 7. 如果是 classdef
  if (ctx->classdef() != nullptr) {
    return visitClassdef(ctx->classdef());
  }
  // 8. 如果是 decorated
  if (ctx->decorated() != nullptr) {
    return visitDecorated(ctx->decorated());
  }
  // 9. 如果是 async_stmt
  if (ctx->async_stmt() != nullptr) {
    return visitAsync_stmt(ctx->async_stmt());
  }
  // 10. 如果是 match_stmt
  if (ctx->match_stmt() != nullptr) {
    return visitMatch_stmt(ctx->match_stmt());
  }
  // 其他情况（不应该出现）

  ConsoleTerminal::get_instance().error(
    "visitCompound_stmt: Unknown compound statement type"
  );
  return nullptr;
}

antlrcpp::Any Generator::visitFuncdef(Python3Parser::FuncdefContext* ctx) {
  auto funcName = Object::PyString::Create(ctx->name()->getText().c_str());
  auto parameters =
    std::any_cast<Object::PyListPtr>(visitParameters(ctx->parameters()));
  auto funcDef =
    IR::CreateFuncDef(funcName, parameters, Object::CreatePyList(), context)
      ->as<IR::FuncDef>();
  auto oldContext = context;  // 保存当前上下文
  context = funcDef;          // 设置当前上下文为函数定义
  auto body = std::any_cast<Object::PyListPtr>(visitBlock(ctx->block()));
  funcDef->SetBody(body);
  context = oldContext;  // 恢复上下文
  return std::dynamic_pointer_cast<IR::INode>(funcDef);
}

antlrcpp::Any Generator::visitBlock(Python3Parser::BlockContext* ctx) {
  auto statements = ctx->stmt();
  if (!statements.empty()) {
    Collections::List<Object::PyObjPtr> stmts(
      static_cast<uint64_t>(statements.size())
    );
    for (auto* statement : statements) {
      auto stmt = visitStmt(statement);
      if (stmt.type() == typeid(IR::INodePtr)) {
        stmts.Push(std::any_cast<IR::INodePtr>(stmt));
        continue;
      }
      if (stmt.type() == typeid(Object::PyListPtr)) {
        auto list = std::any_cast<Object::PyListPtr>(stmt);
        for (Index i = 0; i < list->Length(); i++) {
          stmts.Push(list->GetItem(i));
        }
        continue;
      }
      ConsoleTerminal::get_instance().error(
        "visitBlock: Unknown statement type"
      );
    }
    return Object::CreatePyList(stmts);
  }
  auto* simple_stmts = ctx->simple_stmts();
  if (simple_stmts != nullptr) {
    return visitSimple_stmts(simple_stmts);
  }
  return nullptr;
}

antlrcpp::Any Generator::visitSimple_stmts(
  Python3Parser::Simple_stmtsContext* ctx
) {
  auto simple_stmt = ctx->simple_stmt();
  if (!simple_stmt.empty()) {
    Collections::List<Object::PyObjPtr> stmts(
      static_cast<uint64_t>(simple_stmt.size())
    );
    for (auto* stmt : simple_stmt) {
      stmts.Push(std::any_cast<IR::INodePtr>(visitSimple_stmt(stmt)));
    }
    return Object::CreatePyList(stmts);
  }
  return nullptr;
}

antlrcpp::Any Generator::visitReturn_stmt(
  Python3Parser::Return_stmtContext* ctx
) {
  if (ctx->testlist() == nullptr) {
    return IR::CreateReturnStmt(
      CreateAtom(Object::CreatePyNone(), context), context
    );
  }
  return IR::CreateReturnStmt(
    std::any_cast<IR::INodePtr>(visitTestlist(ctx->testlist())), context
  );
}

antlrcpp::Any Generator::visitTestlist(Python3Parser::TestlistContext* ctx) {
  return visitTest(ctx->test(0));
}

antlrcpp::Any Generator::visitParameters(
  Python3Parser::ParametersContext* ctx
) {
  if (ctx->typedargslist() != nullptr) {
    return visitTypedargslist(ctx->typedargslist());
  }
  return Object::CreatePyList();
}

antlrcpp::Any Generator::visitTypedargslist(
  Python3Parser::TypedargslistContext* ctx
) {
  auto tfpdef = ctx->tfpdef();
  Collections::List<Object::PyObjPtr> args(
    static_cast<uint64_t>(tfpdef.size())
  );
  for (auto* def : tfpdef) {
    args.Push(Object::PyString::Create(def->getText().c_str()));
  }
  return Object::CreatePyList(args);
}

antlrcpp::Any Generator::visitTfpdef(Python3Parser::TfpdefContext* ctx) {
  return Object::PyString::Create(ctx->getText().c_str());
}

antlrcpp::Any Generator::visitIf_stmt(Python3Parser::If_stmtContext* ctx) {
  auto condition = std::any_cast<IR::INodePtr>(visitTest(ctx->test(0)));
  auto thenStmts = std::any_cast<Object::PyListPtr>(visitBlock(ctx->block(0)));
  Object::PyListPtr elseStmts = Object::CreatePyList();
  Object::PyListPtr elseIfStmts = Object::CreatePyList();
  Object::PyListPtr elseIfConditions = Object::CreatePyList();
  size_t elseifCount = ctx->ELIF().size();
  bool hasElse = ctx->ELSE() != nullptr;
  if (elseifCount > 0) {
    for (size_t i = 0; i < elseifCount; ++i) {
      auto elseIfCondition =
        std::any_cast<IR::INodePtr>(visitTest(ctx->test(i + 1)));
      elseIfConditions->Append(elseIfCondition);
      auto block =
        std::any_cast<Object::PyListPtr>(visitBlock(ctx->block(i + 1)));
      elseIfStmts->Append(block);
    }
  }
  if (hasElse) {
    elseStmts =
      std::any_cast<Object::PyListPtr>(visitBlock(ctx->block(elseifCount + 1)));
  }

  auto ifStmt = IR::CreateIfStmt(
    condition, thenStmts, elseStmts, elseIfStmts, elseIfConditions, context
  );
  return ifStmt;
}

antlrcpp::Any Generator::visitWhile_stmt(
  Python3Parser::While_stmtContext* ctx
) {
  auto condition = std::any_cast<IR::INodePtr>(visitTest(ctx->test()));
  auto body = std::any_cast<Object::PyListPtr>(visitBlock(ctx->block(0)));
  return IR::CreateWhileStmt(condition, body, context);
}

// 返回一个INodePtr
antlrcpp::Any Generator::visitSubscriptlist(
  Python3Parser::SubscriptlistContext* ctx
) {
  if (ctx->subscript_().size() == 1) {
    return std::any_cast<IR::INodePtr>(visitSubscript_(ctx->subscript_(0)));
  }
  Collections::List<Object::PyObjPtr> subscripts(
    static_cast<uint64_t>(ctx->subscript_().size())
  );
  for (auto* subscript : ctx->subscript_()) {
    subscripts.Push(std::any_cast<IR::INodePtr>(visitSubscript_(subscript)));
  }
  return IR::CreateList(Object::CreatePyList(subscripts), context);
}

antlrcpp::Any Generator::visitSubscript_(
  Python3Parser::Subscript_Context* ctx
) {
  if (ctx->COLON() == nullptr) {
    return std::any_cast<IR::INodePtr>(visitTest(ctx->test(0)));
  }
  auto none = IR::CreateAtom(Object::CreatePyNone(), context);
  auto step = none;
  if (ctx->sliceop() != nullptr) {
    step = std::any_cast<IR::INodePtr>(visitSliceop(ctx->sliceop()));
  }
  // 存在冒号
  // 如果只有一个test
  // 判断是左边界还是右边界
  if (ctx->test().size() == 1) {
    auto* test = ctx->test(0);
    auto* colon = ctx->COLON();
    auto value = std::any_cast<IR::INodePtr>(visitTest(test));
    if (colon->getSymbol()->getTokenIndex() <
        test->getStart()->getTokenIndex()) {
      return IR::CreateSlice(
        Object::CreatePyList({none, value, step}), context
      );
    }
    return IR::CreateSlice(Object::CreatePyList({value, none, step}), context);
  }
  if (ctx->test().size() == 2) {
    auto start = std::any_cast<IR::INodePtr>(visitTest(ctx->test(0)));
    auto stop = std::any_cast<IR::INodePtr>(visitTest(ctx->test(1)));
    return IR::CreateSlice(Object::CreatePyList({start, stop, step}), context);
  }
  return IR::CreateSlice(Object::CreatePyList({none, none, step}), context);
}

antlrcpp::Any Generator::visitFor_stmt(Python3Parser::For_stmtContext* ctx) {
  auto target = std::any_cast<IR::INodePtr>(visitExprlist(ctx->exprlist()));
  auto iter = std::any_cast<IR::INodePtr>(visitTestlist(ctx->testlist()));
  auto body = std::any_cast<Object::PyListPtr>(visitBlock(ctx->block(0)));
  return IR::CreateForStmt(target, iter, body, context);
}

antlrcpp::Any Generator::visitExprlist(Python3Parser::ExprlistContext* ctx) {
  return visitExpr(ctx->expr(0));
}

antlrcpp::Any Generator::visitClassdef(Python3Parser::ClassdefContext* ctx) {
  auto className = Object::PyString::Create(ctx->name()->getText().c_str());
  auto bases = Object::CreatePyList();
  if (ctx->arglist() != nullptr) {
    bases = std::any_cast<Object::PyListPtr>(visitArglist(ctx->arglist()));
  }
  if (bases->Length() == 0) {
    bases->Append(
      IR::CreateIdentifier(Object::PyString::Create("object"), context)
    );
  }
  auto classDef =
    IR::CreateClassDef(className, IR::CreateList(bases, context), context)
      ->as<IR::ClassDef>();
  auto oldContext = context;
  context = classDef;
  auto body = std::any_cast<Object::PyListPtr>(visitBlock(ctx->block()));
  classDef->SetBody(body);
  context = oldContext;
  return std::dynamic_pointer_cast<IR::INode>(classDef);
}

antlrcpp::Any
Generator::visitPass_stmt(Python3Parser::Pass_stmtContext* /*ctx*/) {
  return IR::CreatePassStmt(context);
}

antlrcpp::Any Generator::visitImport_stmt(
  Python3Parser::Import_stmtContext* /*ctx*/
) {
  return IR::CreateAtom(Object::CreatePyNone(), context);
}

antlrcpp::Any Generator::visitYield_stmt(
  Python3Parser::Yield_stmtContext* ctx
) {
  return IR::CreateExprStmt(
    std::any_cast<IR::INodePtr>(visitYield_expr(ctx->yield_expr())), context
  );
}

antlrcpp::Any Generator::visitYield_expr(
  Python3Parser::Yield_exprContext* ctx
) {
  if (ctx->yield_arg()->testlist() != nullptr) {
    return IR::CreateYieldExpr(
      std::any_cast<IR::INodePtr>(visitTestlist(ctx->yield_arg()->testlist())),
      context
    );
  }
  return IR::CreateYieldExpr(
    IR::CreateAtom(Object::CreatePyNone(), context), context
  );
}

}  // namespace kaubo::Generation