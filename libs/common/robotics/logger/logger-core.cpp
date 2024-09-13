#include "logger-core.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "../platform/thread.hpp"
#include "../utils/no_mutex_lifo.hpp"

namespace {
const size_t kLogRingBufferSize = 0x8000;  // 32KB
const size_t kLogLineSize = 0x200;         // 512 bytes
const size_t kMaxLogLines = 200;

char log_ring_buffer[kLogRingBufferSize];
char log_line[kLogLineSize];

size_t log_head = 0;

void UseLine(size_t start, size_t length) {
  memset(log_line, 0, sizeof(log_line));
  for (size_t offset = 0; offset < length; offset++) {
    size_t logical_index = start + offset;
    size_t index = logical_index % kLogRingBufferSize;

    log_line[offset] = log_ring_buffer[index];
    log_ring_buffer[index] = 0;
  }
}

size_t PasteToRing(const char* line, size_t length) {
  size_t start = log_head;
  for (size_t offset = 0; offset < length; offset++) {
    size_t logical_index = start + offset;
    size_t index = logical_index % kLogRingBufferSize;

    log_ring_buffer[index] = line[offset];
  }

  log_head = (start + length) % kLogRingBufferSize;

  return start;
}
}  // namespace

namespace robotics::logger::core {

struct LogLine {
  size_t data_start = 0;
  size_t length = 0;
  Level level;
  LogLine(const char* data = nullptr, Level level = Level::kInfo)
      : level(level) {
    if (data) {
      data_start = PasteToRing(data, strlen(data));
      length = strlen(data);
    }
  }

  void operator=(const char* str) {
    data_start = PasteToRing(str, strlen(str));
    length = strlen(str);
  }
};

using LogQueue = robotics::utils::NoMutexLIFO<LogLine, kMaxLogLines>;

LogQueue* log_queue = nullptr;

robotics::system::Thread* logger_thread;

void Log(Level level, char* line) {
  if (!log_queue) return;

  LogLine line_{line, level};

  log_queue->Push(line_);
}

void LogHex(Level level, const uint8_t* data, size_t length) {
  static char buffer[kLogLineSize];
  if (!log_queue) return;

  LogLine line{nullptr, level};
  line.length = 0;

  for (size_t i = 0; i < length; i++) {
    line.length += snprintf(buffer + line.length, sizeof(buffer) - line.length,
                            "%02X", data[i]);
  }

  line = buffer;

  log_queue->Push(line);
}

void LoggerProcess() {
  static char level_header[12];
  if (!log_queue) {
    return;
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

    UseLine(line.data_start, line.length);
    printf("%s %s\n", level_header, log_line);
  }
}

void Thread() {
  while (true) {
    LoggerProcess();
  }
}

void Init() {
  if (log_queue) return;

  log_queue = new LogQueue();
  while (!log_queue->Full()) {
    log_queue->Push({""});
  }

  while (!log_queue->Empty()) {
    log_queue->Pop();
  }
}

void StartLoggerThread() {
  Init();

  if (logger_thread) return;
  logger_thread = new robotics::system::Thread();
  logger_thread->Start(Thread);
}

}  // namespace robotics::logger::core