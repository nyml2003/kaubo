#pragma once

#include <cstdint>
namespace kaubo::Json {
enum class ParseError : uint8_t {
  UnexpectedToken,
  UnexpectedEndOfInput,
  InvalidNumberFormat,
  MissingColonInObject,
  MissingCommaOrBracket,
  MissingQuote,
  InvalidEscapeSequence
};

}  // namespace kaubo::Json

namespace std {

inline auto to_string(kaubo::Json::ParseError error) -> const char* {
  switch (error) {
    case kaubo::Json::ParseError::UnexpectedToken:
      return "Unexpected token";
    case kaubo::Json::ParseError::UnexpectedEndOfInput:
      return "Unexpected end of input";
    case kaubo::Json::ParseError::InvalidNumberFormat:
      return "Invalid number format";
    case kaubo::Json::ParseError::MissingColonInObject:
      return "Missing colon in object";
    case kaubo::Json::ParseError::MissingCommaOrBracket:
      return "Missing comma or bracket";
    case kaubo::Json::ParseError::MissingQuote:
      return "Missing quote";
    case kaubo::Json::ParseError::InvalidEscapeSequence:
      return "Invalid escape sequence";
  }
}
}  // namespace std