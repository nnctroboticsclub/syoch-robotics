#include <logger/logger-core.hpp>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>

#include <robotics/utils/span.hpp>

#ifdef USE_THREAD
#include <robotics/thread/thread.hpp>
#endif

#ifdef LOG_FOR_MBED
#include <utils/mbed/target_detector.hpp>
#endif

#include <robotics/utils/no_mutex_lifo.hpp>

#include <logger/log_sink.hpp>

using robotics::utils::Span;

namespace {
#if defined(LOG_FOR_MBED)
#if UtilsMbed_TargetIs(NUCLEO_F303K8)
const size_t kLogRingBufferSize = 0x200;
const size_t kLogLineSize = 0x80;
const size_t kMaxLogLines = 1;
#else   // ^ NUCLEO_F303K8 ,v NUCLEO_F446RE
const size_t kLogRingBufferSize = 0x8000;
const size_t kLogLineSize = 0x100;
const size_t kMaxLogLines = 200;
#endif  // ^ NUCLEO_F303K8
#else   // ^ LOG_FOR_MBED
const size_t kLogRingBufferSize = 0x8000;
const size_t kLogLineSize = 0x100;
const size_t kMaxLogLines = 200;
#endif

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

Span<char> PasteToRing(Span<const char> text) {
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
  Span<const char> tag;
  Span<const char> msg;

  LogLine() = default;

  LogLine(Span<const char> tag, Span<const char> msg,
          Level level = Level::kInfo)
      : level(level),           //
        tag(PasteToRing(tag)),  //
        msg(PasteToRing(msg)) {}
};

using LogQueue = robotics::utils::NoMutexLIFO<LogLine, kMaxLogLines>;
static LogQueue* log_queue = nullptr;

void Log(Level level, const char* tag, const char* msg) {
  if (!log_queue)
    return;

  log_queue->Push(LogLine({tag, strlen(tag)}, {msg, strlen(msg)}, level));
}

void LogHex(Level level, const char* tag, uint8_t* data, uint32_t length) {
  static char buffer[kLogLineSize];
  if (!log_queue)
    return;

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

  std::array<char, 64> tag_buf;
  std::array<char, kLogLineSize> msg_buf;

  while (!log_queue->Empty()) {
    auto line = log_queue->Pop();

    UseLine(line.tag.data() - log_ring_buffer, line.tag.size(), tag_buf.data());
    UseLine(line.msg.data() - log_ring_buffer, line.msg.size(), msg_buf.data());

    global_log_sink->Log(line.level, tag_buf.data(), msg_buf.data());
  }
}

[[noreturn]] static void Thread() {
  while (true) {
    LoggerProcess();
  }
}

void Init() {
  LogLine dummy_log_line({}, {});
  if (log_queue)
    return;

  printf("Allocating %d bytes for log queue\n", sizeof(LogQueue));

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

  if (logger_thread)
    return;
  logger_thread = new robotics::system::Thread();
  logger_thread->SetThreadName("Logger");
  logger_thread->Start(Thread);
}

#endif

}  // namespace robotics::logger::core