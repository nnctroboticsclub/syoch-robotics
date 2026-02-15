#pragma once

#include <cstddef>
#include "Nano/no_mutex_lifo.hpp"
#include "Nano/span.hpp"

#include <NanoHW/thread.hpp>

#include "./log_line.hpp"
#include "./log_sink.hpp"
#include "./logger-core.hpp"

namespace robotics::logger::details {
using core::LogLine;
using Nano::collection::NoMutexLIFO;
using Nano::collection::Span;

// 提供する関数の宣言
void UseLineImpl(size_t start, size_t length, char* dest);
Span<char> PasteToRingImpl(Span<const char> text);
void PushLineImpl(LogLine& log);
void ClearQueueImpl();
void LoggerProcessImpl();
void StartLoggerThread_Impl_();

/// @brief ロガーにバッファ実装を注入するクラス
/// @details
///   Friend injection を用いて、リンク時の注入を行っている
///   利用側は template class LoggerBufferImpl<SomeConfigClass> とすれば良い
template <typename Config>
class LoggerBufferImpl {
  static inline char log_ring_buffer[Config::kLogRingBufferSize] = {};
  static inline size_t log_head = 0;
  static inline NoMutexLIFO<LogLine, Config::kMaxLogLines> log_queue;

  friend void UseLineImpl(size_t start, size_t length, char* dest) {
    size_t offset;

    for (offset = 0; offset < length; offset++) {
      size_t index = (start + offset) % Config::kLogRingBufferSize;

      dest[offset] = log_ring_buffer[index];
    }

    dest[offset] = 0;
  }

  friend Span<char> PasteToRingImpl(Span<const char> text) {
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

// Non-threading 環境用のダミー
__attribute__((weak)) void StartLoggerThread_Impl_() {}

/// @brief ロガーにスレッド実装を注入するクラス
/// @details
///   Friend injection を用いて、リンク時の注入を可能にしてる
///   利用側は template class LoggerThreadImpl<SomeThreadClass> とすれば良い
template <nano_hw::thread::Thread Thread>
class LoggerThreadImpl {
  static Thread* logger_thread;
  [[noreturn]] static void Task() {
    while (true) {
      LoggerProcess();
    }
  }

 public:
  friend void StartLoggerThread_Impl_() {
    logger_thread = new Thread(ThreadPriorityNormal, 1024, nullptr, "Logger");
    logger_thread->Start(std::function<void()>(Task));
  }
};

//* Template class 内で friend 定義された関数はオブジェクトファイルに含まれないため、通常の関数を挟み実体を残す
void UseLine(size_t start, size_t length, char* dest) {
  UseLineImpl(start, length, dest);
}
Span<char> PasteToRing(Span<const char> text) {
  return PasteToRingImpl(text);
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

/// @brief F303K8 用のデフォルト設定
struct DefaultConfig_F303K8 {
  static inline constexpr size_t kLogRingBufferSize = 0x100;
  static inline constexpr size_t kLogLineSize = 0x80;
  static inline constexpr size_t kMaxLogLines = 3;
};

/// @brief F446RE 用のデフォルト設定
struct DefaultConfig_F446RE {
  static inline constexpr size_t kLogRingBufferSize = 0x8000;
  static inline constexpr size_t kLogLineSize = 0x100;
  static inline constexpr size_t kMaxLogLines = 200;
};

}  // namespace robotics::logger::details