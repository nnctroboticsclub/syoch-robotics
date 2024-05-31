#include "char_logger.hpp"

#include <cstring>

namespace robotics::logger {
CharLogger::CharLogger(const char* id, const char* tag) : logger(id, tag) {}

void CharLogger::SetLevel(core::Level level) { this->level = level; }

void CharLogger::SetCachingMode(CachingMode mode) { caching_mode = mode; }

void CharLogger::Log(char c) {
  if (caching_mode == CachingMode::kNone) {
    logger.Log(level, "%c", c);
  }

  if (caching_mode == CachingMode::kNewLine) {
    if (c == '\n' || strlen(data) >= sizeof(data) - 1) {
      logger.Log(level, "%s", data);
      memset(data, 0, sizeof(data));
      return;
    } else if (c == '\r') {
      return;
    } else if (isprint(c)) {
      data[strlen(data)] = c;
    } else {
      data[strlen(data)] = '\\';
      data[strlen(data)] = 'x';
      data[strlen(data)] = "0123456789ABCDEF"[c >> 4];
      data[strlen(data)] = "0123456789ABCDEF"[c & 0xF];
    }
  }
}

}  // namespace robotics::logger
