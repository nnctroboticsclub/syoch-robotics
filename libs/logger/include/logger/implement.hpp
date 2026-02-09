#pragma once

#include <cstddef>
#include "robotics/utils/no_mutex_lifo.hpp"
#include "robotics/utils/span.hpp"

#include <robotics/thread/thread.hpp>

#include "./log_sink.hpp"
#include "./logger-core.hpp"

namespace robotics::logger::details {
using core::LogLine;
using utils::Span;

//* Optionally implemented function
void UseLineImpl(size_t start, size_t length, char* dest);
utils::Span<char> PasteToRingImpl(utils::Span<const char> text);
void PushLineImpl(LogLine& log);
void ClearQueueImpl();
void LoggerProcessImpl();
void StartLoggerThread_Impl_();

__attribute__((weak)) void StartLoggerThread_Impl_() {}

template <typename Config>
class LoggerBufferImpl {
  static inline char log_ring_buffer[Config::kLogRingBufferSize] = {};
  static inline size_t log_head = 0;
  static inline robotics::utils::NoMutexLIFO<LogLine, Config::kMaxLogLines>
      log_queue;

  friend void UseLineImpl(size_t start, size_t length, char* dest) {
    size_t offset;

    for (offset = 0; offset < length; offset++) {
      size_t index = (start + offset) % Config::kLogRingBufferSize;

      dest[offset] = log_ring_buffer[index];
    }

    dest[offset] = 0;
  }

  friend utils::Span<char> PasteToRingImpl(utils::Span<const char> text) {
    size_t start = log_head;
    for (size_t offset = 0; offset < text.size(); offset++) {
      size_t index = (start + offset) % Config::kLogRingBufferSize;

      log_ring_buffer[index] = text[offset];
    }

    log_head = (start + text.size()) % Config::kLogRingBufferSize;

    return {log_ring_buffer + start, text.size()};
  }

  friend void PushLineImpl(LogLine& log) { log_queue.Push(log); }

  friend void ClearQueueImpl() {
    log_queue.ClearDatas();
    log_queue.Clear();
  }

  friend void LoggerProcessImpl() {
    ::std::array<char, 64> tag_buf;
    ::std::array<char, Config::kLogLineSize> msg_buf;

    while (!log_queue.Empty()) {
      auto line = log_queue.Pop();

      UseLineImpl(line.tag.data() - log_ring_buffer, line.tag.size(),
                  tag_buf.data());
      UseLineImpl(line.msg.data() - log_ring_buffer, line.msg.size(),
                  msg_buf.data());

      global_log_sink->Log(line.level, tag_buf.data(), msg_buf.data());
    }
  }
};

template <int dummy>
class LoggerThread {
  static robotics::system::Thread* logger_thread;

  [[noreturn]] static void Thread() {
    while (true) {
      LoggerProcess();
    }
  }

 public:
  friend void StartLoggerThread_Impl_() {
    if (logger_thread)
      return;
    logger_thread = new robotics::system::Thread();
    logger_thread->SetThreadName("Logger");
    logger_thread->Start(std::function<void()>(Thread));
  }
};

//* Instantiated functions
void UseLine(size_t start, size_t length, char* dest) {
  UseLineImpl(start, length, dest);
}
utils::Span<char> PasteToRing(utils::Span<const char> text) {
  PasteToRingImpl(text);
}
void PushLine(LogLine& log) {
  PushLineImpl(log);
}
void ClearQueue() {
  ClearQueueImpl();
}
void LoggerProcess() {
  LoggerProcessImpl();
}

}  // namespace robotics::logger::details