#pragma once

#include "Lexer/Core/Builder.h"
#include "Lexer/Core/Proto.h"
#include "Tools/Json/Lexer/TokenType.h"


namespace kaubo::Json {
class Builder : public Lexer::IBuilder<Json::TokenType, Builder> {
 public:
  auto build() -> Lexer::Instance<TokenType> override;
};

}  // namespace kaubo::Json