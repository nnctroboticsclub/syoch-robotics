#pragma once

#include <stddef.h>
#include <stdint.h>
#include "robotics/utils/span.hpp"

namespace robotics::logger::details {

void LoggerProcess();
void StartLoggerThread_Impl();

template <typename T>
void StartLoggerThread(T _) {
  StartLoggerThread_Impl();
}
}  // namespace robotics::logger::details

namespace robotics::logger::core {
using details::LoggerProcess;

enum class Level : uint8_t { kError, kInfo, kVerbose, kDebug, kTrace };

struct LogLine {
  Level level;
  utils::Span<const char> tag;
  utils::Span<const char> msg;

  LogLine() = default;

  LogLine(utils::Span<const char> tag, utils::Span<const char> msg,
          core::Level level = core::Level::kInfo);
};

void Log(Level level, const char* tag, const char* msg);
void LogHex(Level level, const char* tag, uint8_t data, uint32_t len);

void Init();

inline void StartLoggerThread() {
  details::StartLoggerThread(0);
}

}  // namespace robotics::logger::core