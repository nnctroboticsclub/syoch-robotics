#pragma once

#include <stdint.h>

namespace robotics::logger::core {
enum class Level : uint8_t { kError, kInfo, kVerbose, kDebug, kTrace };

}  // namespace robotics::logger::core