#pragma once

#include "IR/INode.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-dtor"

#include "Python3ParserBaseVisitor.h"

#pragma clang diagnostic pop

namespace kaubo::Generation {

class Generator : public Python3ParserBaseVisitor {
 private:
  Object::PyListPtr codeList;
  IR::INodePtr context;

 public:
  explicit Generator(const Object::PyStrPtr& filename);
  void Visit();
  void Emit();
  void Print();
  [[nodiscard]] Object::PyCodePtr Code() const;
  antlrcpp::Any visitFile_input(Python3Parser::File_inputContext* ctx) override;
  antlrcpp::Any visitAtom(Python3Parser::AtomContext* ctx) override;
  antlrcpp::Any visitExpr(Python3Parser::ExprContext* ctx) override;

  antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext* ctx) override;

  antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext* ctx) override;

  antlrcpp::Any visitArglist(Python3Parser::ArglistContext* ctx) override;

  antlrcpp::Any visitTestlist_star_expr(
    Python3Parser::Testlist_star_exprContext* ctx
  ) override;

  antlrcpp::Any visitArgument(Python3Parser::ArgumentContext* ctx) override;

  antlrcpp::Any visitTest(Python3Parser::TestContext* ctx) override;

  antlrcpp::Any visitOr_test(Python3Parser::Or_testContext* ctx) override;

  antlrcpp::Any visitAnd_test(Python3Parser::And_testContext* ctx) override;

  antlrcpp::Any visitNot_test(Python3Parser::Not_testContext* ctx) override;

  antlrcpp::Any visitComparison(Python3Parser::ComparisonContext* ctx) override;

  antlrcpp::Any visitTestlist_comp(
    Python3Parser::Testlist_compContext* ctx
  ) override;

  antlrcpp::Any visitCompound_stmt(
    Python3Parser::Compound_stmtContext* ctx
  ) override;

  antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext* ctx) override;

  antlrcpp::Any visitBlock(Python3Parser::BlockContext* ctx) override;

  antlrcpp::Any visitSimple_stmts(
    Python3Parser::Simple_stmtsContext* ctx
  ) override;

  antlrcpp::Any visitStmt(Python3Parser::StmtContext* ctx) override;

  antlrcpp::Any visitReturn_stmt(
    Python3Parser::Return_stmtContext* ctx
  ) override;

  antlrcpp::Any visitTestlist(Python3Parser::TestlistContext* ctx) override;

  antlrcpp::Any visitParameters(Python3Parser::ParametersContext* ctx) override;

  antlrcpp::Any visitTypedargslist(
    Python3Parser::TypedargslistContext* ctx
  ) override;

  antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext* ctx) override;

  antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext* ctx) override;

  antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext* ctx) override;

  antlrcpp::Any visitSubscriptlist(
    Python3Parser::SubscriptlistContext* ctx
  ) override;

  antlrcpp::Any visitSubscript_(Python3Parser::Subscript_Context* ctx) override;

  antlrcpp::Any visitFor_stmt(Python3Parser::For_stmtContext* ctx) override;

  antlrcpp::Any visitExprlist(Python3Parser::ExprlistContext* ctx) override;

  antlrcpp::Any visitClassdef(Python3Parser::ClassdefContext* ctx) override;

  antlrcpp::Any visitPass_stmt(Python3Parser::Pass_stmtContext* ctx) override;

  antlrcpp::Any visitYield_stmt(Python3Parser::Yield_stmtContext* ctx) override;

  antlrcpp::Any visitYield_expr(Python3Parser::Yield_exprContext* ctx) override;

  antlrcpp::Any visitImport_stmt(
    Python3Parser::Import_stmtContext* ctx
  ) override;
};

}  // namespace kaubo::Generation
