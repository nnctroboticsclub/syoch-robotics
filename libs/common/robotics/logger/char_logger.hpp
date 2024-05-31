#pragma once

#include "generic_logger.hpp"

#include <cctype>

namespace robotics::logger {
struct CharLogger {
  GenericLogger logger;
  char data[512] = {};

  core::Level level = core::Level::kInfo;

  enum class CachingMode {
    kNone,
    kNewLine,
  } caching_mode = CachingMode::kNewLine;

 public:
  CharLogger(const char* id, const char* tag);

  void SetLevel(core::Level level);

  void SetCachingMode(CachingMode mode);

  void Log(char c);
};
}  // namespace robotics::logger
