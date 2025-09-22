#pragma once

#include "Stmt.h"
namespace kaubo::Parser {
struct Module {
  std::vector<StmtPtr> statements;
};

using ModulePtr = std::shared_ptr<Module>;
}  // namespace kaubo::Parser