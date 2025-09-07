#pragma once

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
#include "IR/Identifier.h"
#include "IR/MemberAccess.h"
#include "IR/Module.h"
#include "IR/Statement/ExprStmt.h"
#include "IR/Statement/ForStmt.h"
#include "IR/Statement/IfStmt.h"
#include "IR/Statement/PassStmt.h"
#include "IR/Statement/ReturnStmt.h"
#include "IR/Statement/WhileStmt.h"

namespace kaubo::IR {

inline void RegisterIRClasses() {
  AtomKlass::Self()->Initialize();
  BinaryKlass::Self()->Initialize();
  FunctionCallKlass::Self()->Initialize();
  ListKlass::Self()->Initialize();
  MapKlass::Self()->Initialize();
  SliceKlass::Self()->Initialize();
  UnaryKlass::Self()->Initialize();
  ExprStmtKlass::Self()->Initialize();
  ForStmtKlass::Self()->Initialize();
  IfStmtKlass::Self()->Initialize();
  PassStmtKlass::Self()->Initialize();
  ReturnStmtKlass::Self()->Initialize();
  WhileStmtKlass::Self()->Initialize();
  YieldExprKlass::Self()->Initialize();
  AssignStmtKlass::Self()->Initialize();
  ClassDefKlass::Self()->Initialize();
  FuncDefKlass::Self()->Initialize();
  IdentifierKlass::Self()->Initialize();
  MemberAccessKlass::Self()->Initialize();
  ModuleKlass::Self()->Initialize();
}

}  // namespace kaubo::IR
