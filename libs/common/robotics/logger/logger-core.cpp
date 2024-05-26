#include "logger-core.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "../platform/thread.hpp"
#include "../utils/no_mutex_lifo.hpp"

namespace robotics::logger::core {

struct LogLine {
  size_t length = 0;
  char data[512] = {};
  Level level;
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

using LogQueue = robotics::utils::NoMutexLIFO<LogLine, 64>;

LogQueue* log_queue = nullptr;

robotics::system::Thread* logger_thread;

void Log(Level level, char* line) {
  if (!log_queue) return;

  LogLine line_{line, level};

  log_queue->Push(line_);
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

void Thread() {
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

      printf("%s %s\n", level_header, line.data);
    }
  }
}

void StartLoggerThread() {
  log_queue = new LogQueue();
  while (!log_queue->Full()) {
    log_queue->Push({""});
  }

  while (!log_queue->Empty()) {
    log_queue->Pop();
  }

  logger_thread = new robotics::system::Thread();
  logger_thread->Start(Thread);
}

}  // namespace robotics::logger::core