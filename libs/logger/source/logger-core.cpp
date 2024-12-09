#include <logger/logger-core.hpp>

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <span>

#include <tcb/span.hpp>

#ifdef USE_THREAD
#include <robotics/thread/thread.hpp>
#endif

#include <robotics/utils/no_mutex_lifo.hpp>

#include <logger/log_sink.hpp>

namespace {
const size_t kLogRingBufferSize = 0x8000;  // 32KB
const size_t kLogLineSize = 0x200;         // 512 bytes
const size_t kMaxLogLines = 200;

char log_ring_buffer[kLogRingBufferSize] = {};

size_t log_head = 0;

void UseLine(size_t start, size_t length, char* dest) {
  size_t offset;

  for (offset = 0; offset < length; offset++) {
    size_t index = (start + offset) % kLogRingBufferSize;

    dest[offset] = log_ring_buffer[index];
  }

  dest[offset] = 0;  // null-terminate for c-style strings
}

tcb::span<char> PasteToRing(tcb::span<const char> text) {
  size_t start = log_head;
  for (size_t offset = 0; offset < text.size(); offset++) {
    size_t index = (start + offset) % kLogRingBufferSize;

    log_ring_buffer[index] = text[offset];
  }

  log_head = (start + text.size()) % kLogRingBufferSize;

  return {log_ring_buffer + start, text.size()};
}
}  // namespace

namespace robotics::logger::core {

struct LogLine {
  Level level;
  tcb::span<const char> tag;
  tcb::span<const char> msg;

  LogLine() = default;

  LogLine(tcb::span<const char> tag, tcb::span<const char> msg,
          Level level = Level::kInfo)
      : level(level), tag(PasteToRing(tag)), msg(PasteToRing(msg)) {}
};

using LogQueue = robotics::utils::NoMutexLIFO<LogLine, kMaxLogLines>;

LogQueue* log_queue = nullptr;

void Log(Level level, const char* tag, const char* msg) {
  if (!log_queue) return;

  log_queue->Push(LogLine({tag, strlen(tag)}, {msg, strlen(msg)}, level));
}

void LogHex(Level level, const char* tag, uint8_t* data, uint32_t length) {
  static char buffer[kLogLineSize];
  if (!log_queue) return;

  auto text_ptr = buffer;
  for (auto* data_ptr = data; data_ptr < data + length; data_ptr++) {
    *(text_ptr++) = "0123456789ABCDEF"[*data_ptr >> 4];
    *(text_ptr++) = "0123456789ABCDEF"[*data_ptr & 0x0F];
  }

  log_queue->Push(LogLine({tag, strlen(tag)}, {buffer, 2 * length}, level));
}

void LoggerProcess() {
  if (!log_queue) {
    return;
  }

  while (!log_queue->Empty()) {
    auto line = log_queue->Pop();

    static char tag_buf[64];
    static char msg_buf[kLogLineSize];

    UseLine(line.tag.data() - log_ring_buffer, line.tag.size(), tag_buf);
    UseLine(line.msg.data() - log_ring_buffer, line.msg.size(), msg_buf);

    global_log_sink->Log(line.level, tag_buf, msg_buf);
  }
}

void Thread() {
  while (true) {
    LoggerProcess();
  }
}

void Init() {
  static auto dummy_log_line = LogLine({}, {});
  if (log_queue) return;

  log_queue = new LogQueue();
  while (!log_queue->Full()) {
    log_queue->Push(dummy_log_line);
  }

  while (!log_queue->Empty()) {
    log_queue->Pop();
  }

#ifdef USE_THREAD
  StartLoggerThread();
#endif
}

#ifdef USE_THREAD

robotics::system::Thread* logger_thread;

void StartLoggerThread() {
  Init();

  if (logger_thread) return;
  logger_thread = new robotics::system::Thread();
  logger_thread->SetThreadName("Logger");
  logger_thread->Start(Thread);
}

#endif

}  // namespace robotics::logger::core