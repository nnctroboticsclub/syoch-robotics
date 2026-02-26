#include <sys/types.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <Nano/no_mutex_lifo.hpp>
#include <Nano/scratch.hpp>
#include <Nano/span.hpp>
#include <logger/log_line.hpp>
#include <logger/log_sink.hpp>
#include <logger/logger-core.hpp>
#include <robotics/binary/context_detector.hpp>

namespace robotics::logger::details {
using core::LogLine;

void UseLine(size_t start, size_t length, char* dest);
Nano::collection::Span<char> PasteToRing(
    Nano::collection::Span<const char> text);
void PushLine(LogLine& log);
void ClearQueue();
void LoggerProcess();
}  // namespace robotics::logger::details

using robotics::logger::details::ClearQueue;
using robotics::logger::details::LogLine;
using robotics::logger::details::PasteToRing;
using robotics::logger::details::PushLine;

namespace robotics::logger::core {
LogLine::LogLine(Nano::collection::Span<const char> tag,
                 Nano::collection::Span<const char> msg, core::Level level)
    : level(level), tag(PasteToRing(tag)), msg(PasteToRing(msg)) {}

void inline Log(Level level, const char* tag, const char* msg, size_t msg_len) {
#if SYNC_PROTECT
  if (robotics::binary::InNonWaitableContext()) {
    auto line = LogLine({tag, strlen(tag)}, {msg, msg_len}, level);
    PushLine(line);
  } else {
    global_log_sink->Log(level, tag, msg);
  }
#else
  global_log_sink->Log(level, tag, msg);
#endif
}

void Log(Level level, const char* tag, const char* msg) {
  Log(level, tag, msg, strlen(msg));
}

void LogHex(Level level, const char* tag, uint8_t* data, uint32_t length) {
  char* buffer =
      reinterpret_cast<char*>(Nano::utils::ScratchBuffer::GetAppBuffer());

  auto text_ptr = buffer;
  for (auto* data_ptr = data; data_ptr < data + length; data_ptr++) {
    *(text_ptr++) = "0123456789ABCDEF"[*data_ptr >> 4];
    *(text_ptr++) = "0123456789ABCDEF"[*data_ptr & 0x0F];
  }

  Log(level, tag, buffer, 2 * length);
}

void Init() {
  ClearQueue();
}

}  // namespace robotics::logger::core