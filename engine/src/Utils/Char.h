#pragma once

namespace kaubo::Utils::Char {

inline auto is_digit(char c) {
  return c >= '0' && c <= '9';
}

inline auto is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline auto is_alphanumeric(char c) {
  return is_digit(c) || is_alpha(c);
}

inline auto is_identifier_start(char c) {
  return is_alphanumeric(c) || c == '_';
}

inline auto is_identifier_part(char c) {
  return is_identifier_start(c) || is_digit(c);
}

}  // namespace kaubo::Utils::Char