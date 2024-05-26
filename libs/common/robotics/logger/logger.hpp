#pragma once

#include <cstdio>
#include <cstdint>
#include <cstring>

#include <cstdarg>

#include "../platform/thread.hpp"
#include "../utils/no_mutex_lifo.hpp"

#include "./logger-core.hpp"

namespace robotics::logger {

class Logger {
  static Logger* loggers[64];

  const char* id;
  const char* tag;

  bool supressed = false;

 private:
  void _Log(core::Level level, const char* fmt, va_list args) {
    if (supressed) return;

    static char line[512] = {};

    char* ptr = line;
    ptr += snprintf(ptr, sizeof(line), "[%s] ", tag);
    ptr += vsnprintf(ptr, sizeof(line) - (ptr - line), fmt, args);

    core::Log(level, line);
  }

 public:
  Logger(const char* id, const char* tag) : id(id), tag(tag) {
    for (int i = 0; i < 64; i++) {
      if (!loggers[i]) {
        Debug("Logger %s initialized as id = %d", id, i);
        loggers[i] = this;
        break;
      }
    }
  }

  void Supress() { supressed = true; }

  void Resume() { supressed = false; }

  void Log(core::Level level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(level, fmt, args);

    va_end(args);
  }

  void Info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(core::Level::kInfo, fmt, args);

    va_end(args);
  }

  void Error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(core::Level::kError, fmt, args);

    va_end(args);
  }

  void Debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(core::Level::kDebug, fmt, args);

    va_end(args);
  }

  void Trace(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(core::Level::kTrace, fmt, args);

    va_end(args);
  }

  void Verbose(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(core::Level::kVerbose, fmt, args);

    va_end(args);
  }

  void Hex(core::Level level, const uint8_t* data, size_t length) {
    core::LogHex(level, data, length);
  }

  static Logger* GetLogger(const char* id) {
    for (int i = 0; i < 64; i++) {
      if (loggers[i] && strcmp(loggers[i]->id, id) == 0) {
        return loggers[i];
      }
    }
    return nullptr;
  }
};

Logger* Logger::loggers[64] = {
    nullptr,
};

struct CharLogger {
  Logger logger;
  char data[512] = {};

  core::Level level = core::Level::kInfo;

  enum class CachingMode {
    kNone,
    kNewLine,
  } caching_mode = CachingMode::kNewLine;

 public:
  CharLogger(const char* id, const char* tag) : logger(id, tag) {}

  void SetLevel(core::Level level) { this->level = level; }

  void SetCachingMode(CachingMode mode) { caching_mode = mode; }

  void Log(char c) {
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
};

Logger system_logger{"system", "logger"};

void SuppressLogger(const char* id) {
  auto logger = Logger::GetLogger(id);
  logger->Supress();
}

void ResumeLogger(const char* id) {
  auto logger = Logger::GetLogger(id);
  logger->Resume();
}

void Init() {
  core::StartLoggerThread();
  system_logger.Info("Logger Initialized (from Log)");
}
}  // namespace robotics::logger
