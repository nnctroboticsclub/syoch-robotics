#pragma once

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

void Log(Level level, const char* fmt, ...) {
  if (!log_queue) return;

  va_list args;
  va_start(args, fmt);

  LogLine line{"", level};
  line.length = vsnprintf(line.data, sizeof(line.data), fmt, args);

  log_queue->Push(line);

  va_end(args);
}

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

struct CharLogGroup {
  char group_tag[16] = {};
  char data[512] = {};

  Level level = Level::kInfo;

  enum class CachingMode {
    kNone,
    kNewLine,
  } caching_mode = CachingMode::kNewLine;

  void SetLevel(Level level) { this->level = level; }

  void SetCachingMode(CachingMode mode) { caching_mode = mode; }

  void AppendChar(char c) {
    if (caching_mode == CachingMode::kNone) {
      Log(level, "[Char#%s] %s", group_tag, data);
    }

    if (caching_mode == CachingMode::kNewLine) {
      if (c == '\n' || strlen(data) >= sizeof(data) - 1) {
        Log(level, "[Char#%s] %s", group_tag, data);
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

CharLogGroup groups[16] = {};

static CharLogGroup* GetCharLogGroup(const char* group_tag) {
  for (auto&& group : groups) {
    if (strcmp(group.group_tag, group_tag) == 0) {
      return &group;
    }
  }

  for (auto&& group : groups) {
    if (strlen(group.group_tag) == 0) {
      strcpy(group.group_tag, group_tag);
      return &group;
    }
  }

  return nullptr;
}

void LogCh(const char* group_tag, char c) {
  auto group = GetCharLogGroup(group_tag);
  if (!group) {
    return;
  }

  group->AppendChar(c);
}

void Init() {
  printf("# Init Logger\n");
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

        printf("[\x1b[33mLOG %2d -%2d]\x1b[m %s %s\n", i, log_queue->Size(),
               level_header, line.data);
      }
    }
  });
  printf("- Logger Thread: %p\n", &logger_thread);

  printf("  - free cells: %d\n", log_queue->Size());

  log_queue->Push(LogLine("Logger Initialized"));
  Log(Level::kInfo, "Logger Initialized (from Log)");
}
}  // namespace robotics::logger