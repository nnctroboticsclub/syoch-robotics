#include "generic_logger.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "../platform/thread.hpp"
#include "../utils/no_mutex_lifo.hpp"

namespace robotics::logger {

static GenericLogger* loggers[64] = {
    nullptr,
};

void GenericLogger::_Log(core::Level level, const char* fmt, va_list args) {
  if (supressed) return;

  static char line[512] = {};

  char* ptr = line;
  ptr += snprintf(ptr, sizeof(line), "[%s] ", tag);
  ptr += vsnprintf(ptr, sizeof(line) - (ptr - line), fmt, args);

  core::Log(level, line);
}

GenericLogger::GenericLogger(const char* id, const char* tag)
    : id(id), tag(tag) {
  for (int i = 0; i < 64; i++) {
    if (!loggers[i]) {
      Debug("Logger %s initialized as id = %d", id, i);
      loggers[i] = this;
      break;
    }
  }
}

void GenericLogger::Supress() { supressed = true; }

void GenericLogger::Resume() { supressed = false; }

void GenericLogger::Log(core::Level level, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(level, fmt, args);

  va_end(args);
}

void GenericLogger::Info(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kInfo, fmt, args);

  va_end(args);
}

void GenericLogger::Error(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kError, fmt, args);

  va_end(args);
}

void GenericLogger::Debug(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kDebug, fmt, args);

  va_end(args);
}

void GenericLogger::Trace(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kTrace, fmt, args);

  va_end(args);
}

void GenericLogger::Verbose(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kVerbose, fmt, args);

  va_end(args);
}

void GenericLogger::Hex(core::Level level, const uint8_t* data, size_t length) {
  if (supressed) return;
  core::LogHex(level, data, length);
}

GenericLogger* GenericLogger::GetLogger(const char* id) {
  for (int i = 0; i < 64; i++) {
    if (loggers[i] && strcmp(loggers[i]->id, id) == 0) {
      return loggers[i];
    }
  }
  return nullptr;
}

}  // namespace robotics::logger
