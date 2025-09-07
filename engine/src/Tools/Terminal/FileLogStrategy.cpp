#include "TerminalStrategy.h"

namespace kaubo {

FileTerminalStrategy::FileTerminalStrategy(const std::string& filename)
  : m_stream(
      filename,
      static_cast<std::ios::openmode>(
        static_cast<unsigned int>(std::ios::out) |
        static_cast<unsigned int>(std::ios::trunc)
      )
    ) {
  if (!m_stream.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }
}

}  // namespace kaubo