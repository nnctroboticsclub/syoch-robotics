#pragma once

#include <stddef.h>
#include <stdint.h>

namespace robotics::logger::core {

enum class Level { kError, kInfo, kVerbose, kDebug, kTrace };

void Log(Level level, char* line);
void LogHex(Level level, const uint8_t* data, size_t length);
void StartLoggerThread();

}  // namespace robotics::logger::core