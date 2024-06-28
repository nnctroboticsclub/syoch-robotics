#include "char_logger.hpp"

#include <cstring>

namespace robotics::logger {
CharLogger::CharLogger(const char* id, const char* tag) : logger(id, tag) {}

void CharLogger::Cache(char c) {
  if (isprint(c)) {
    if (strlen(data) + 1 > sizeof(data)) return;
    data[strlen(data)] = c;
  } else {
    if (strlen(data) + 4 > sizeof(data)) return;
    data[strlen(data)] = '\\';
    data[strlen(data)] = 'x';
    data[strlen(data)] = "0123456789ABCDEF"[c >> 4];
    data[strlen(data)] = "0123456789ABCDEF"[c & 0xF];
  }
}

void CharLogger::SetLevel(core::Level level) { this->level = level; }

void CharLogger::SetCachingMode(CachingMode mode) { caching_mode = mode; }

void CharLogger::SetHeader(char* header) {
  strncpy(this->header, header, sizeof(this->header));
}

void CharLogger::Log(char c) {
  if (caching_mode == CachingMode::kNone) {
    logger.Log(level, "%s%c", header, c);
  } else if (caching_mode == CachingMode::kNewLine) {
    if (c == '\n' || strlen(data) >= sizeof(data) - 1) {
      Flush();
      ClearBuffer();
      return;
    } else if (c == '\r') {
      return;
    } else {
      Cache(c);
    }
  } else if (caching_mode == CachingMode::kUser) {
    Cache(c);
  }
}

void CharLogger::LogN(char* c, size_t n) {
  for (size_t i = 0; i < n; i++) {
    Log(c[i]);
  }
}

void CharLogger::Flush() { logger.Log(level, "%s%s", header, data); }
void CharLogger::ClearBuffer() { memset(data, 0, sizeof(data)); }

}  // namespace robotics::logger
