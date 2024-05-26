#pragma once

#include <cstdio>
#include <cstdint>
#include <cstring>

#include <cstdarg>

#include "../platform/thread.hpp"
#include "../utils/no_mutex_lifo.hpp"

namespace robotics::logger {
struct LogLine {
  size_t length = 0;
  char data[512] = {};

  enum class Level {
    kError,
    kInfo,
    kVerbose,
    kDebug,
    kTrace
  } level = Level::kInfo;

  LogLine(const char* data = nullptr, Level level = Level::kInfo)
      : level(level) {
    if (data) {
      memset(this->data, 0, sizeof(this->data));
      memcpy(this->data, data, strlen(data));
      length = strlen(this->data);
    }
  }

  char* operator=(const char* str) {
    memset(data, 0, sizeof(data));
    memcpy(data, str, strlen(str));
    length = strlen(data);
    return data;
  }
};

using Level = LogLine::Level;

using LogQueue = robotics::utils::NoMutexLIFO<LogLine, 64>;

LogQueue* log_queue = nullptr;

robotics::system::Thread logger_thread;

void LogHex(Level level, const uint8_t* data, size_t length) {
  if (!log_queue) return;

  LogLine line{"", level};
  line.length = 0;

  for (size_t i = 0; i < length; i++) {
    line.length += snprintf(line.data + line.length,
                            sizeof(line.data) - line.length, "%02X", data[i]);
  }

  log_queue->Push(line);
}

class Logger {
  static Logger *loggers[64];

  const char* id;
  const char* tag;

  bool supressed = false;

 private:
  void _Log(Level level, const char* fmt, va_list args) {
    if (supressed) return;
    if (!log_queue) return;

    LogLine line{"", level};

    char* ptr = line.data;
    ptr += snprintf(ptr, sizeof(line.data), "[%s] ", tag);
    ptr += vsnprintf(ptr, sizeof(line.data) - (ptr - line.data), fmt, args);

    line.length = ptr - line.data;

    log_queue->Push(line);
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

  void Log(Level level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(level, fmt, args);

    va_end(args);
  }

  void Info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(Level::kInfo, fmt, args);

    va_end(args);
  }

  void Error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(Level::kError, fmt, args);

    va_end(args);
  }

  void Debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(Level::kDebug, fmt, args);

    va_end(args);
  }

  void Trace(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(Level::kTrace, fmt, args);

    va_end(args);
  }

  void Verbose(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    _Log(Level::kVerbose, fmt, args);

    va_end(args);
  }

  void Hex(Level level, const uint8_t* data, size_t length) {
    LogHex(level, data, length);
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

  Level level = Level::kInfo;

  enum class CachingMode {
    kNone,
    kNewLine,
  } caching_mode = CachingMode::kNewLine;
  public:
  CharLogger(const char *id, const char* tag) : logger(id, tag) {}

  void SetLevel(Level level) { this->level = level; }

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
  if (log_queue) {
    printf("- Logger already initialized\n");
    return;
  }
  printf("- sizeof(LogQueue): %#08x\n", sizeof(LogQueue));
  log_queue = new LogQueue();
  while (!log_queue->Full()) {
    log_queue->Push({""});
  }

  while (!log_queue->Empty()) {
    log_queue->Pop();
  }
  printf("- Log Queue: %p\n", log_queue);

  logger_thread.Start([]() {
    char level_header[12];

    while (1) {
      if (!log_queue) {
        ThisThread::sleep_for(1ms);
        continue;
      }

      for (int i = 0; !log_queue->Empty(); i++) {
        auto line = log_queue->Pop();

        switch (line.level) {
          case Level::kError:
            snprintf(level_header, sizeof(level_header), "\x1b[1;31mE\x1b[m");
            break;
          case Level::kInfo:
            snprintf(level_header, sizeof(level_header), "\x1b[1;32mI\x1b[m");
            break;
          case Level::kVerbose:
            snprintf(level_header, sizeof(level_header), "\x1b[1;34mV\x1b[m");
            break;
          case Level::kDebug:
            snprintf(level_header, sizeof(level_header), "\x1b[1;36mD\x1b[m");
            break;
          case Level::kTrace:
            snprintf(level_header, sizeof(level_header), "\x1b[1;35mT\x1b[m");
            break;

          default:
            snprintf(level_header, sizeof(level_header), "\x1b[1;37m?\x1b[m");
            break;
        }

        printf("%s %s\n",
               level_header, line.data);
      }
    }
  });
  printf("- Logger Thread: %p\n", &logger_thread);

  printf("  - free cells: %d\n", log_queue->Size());

  log_queue->Push(LogLine("Logger Initialized"));
  system_logger.Info("Logger Initialized (from Log)");
}
}  // namespace robotics::logger
