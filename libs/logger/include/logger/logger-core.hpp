#pragma once

#include <stdint.h>

namespace robotics::logger::core {

enum class Level : uint8_t { kError, kInfo, kVerbose, kDebug, kTrace };

void Log(Level level, const char* tag, const char* msg);
void LogHex(Level level, const char* tag, uint8_t data, uint32_t len);

#ifdef USE_THREAD
void StartLoggerThread();
#endif

void LoggerProcess();
void Init();

}  // namespace robotics::logger::core