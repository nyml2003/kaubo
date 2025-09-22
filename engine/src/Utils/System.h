#pragma once

#include <fstream>
#include <string>

namespace kaubo::Utils::System {
inline auto read_file(const char* path) -> std::string {
  std::ifstream file(path);
  std::string content(
    (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()
  );
  return content;
}
}  // namespace kaubo::Utils::System