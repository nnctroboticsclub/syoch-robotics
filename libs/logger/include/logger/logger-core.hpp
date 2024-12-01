#pragma once

#include <tcb/span.hpp>

#include <stddef.h>
#include <stdint.h>

namespace robotics::logger::core {

enum class Level { kError, kInfo, kVerbose, kDebug, kTrace };

void Log(Level level, tcb::span<const char> tag, tcb::span<const char> msg);
void LogHex(Level level, tcb::span<const char> tag, tcb::span<uint8_t> msg);

#ifdef USE_THREAD
void StartLoggerThread();
#endif

void LoggerProcess();
void Init();

}  // namespace robotics::logger::core