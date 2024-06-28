#pragma once

#include "generic_logger.hpp"

#include <cctype>

namespace robotics::logger {
struct CharLogger {
  GenericLogger logger;
  char data[512] = {};
  char header[32] = {0};

  core::Level level = core::Level::kInfo;

  enum class CachingMode {
    kNone,
    kNewLine,
    kUser
  } caching_mode = CachingMode::kNewLine;

  void Cache(char c);

 public:
  CharLogger(const char* id, const char* tag);

  void SetLevel(core::Level level);
  void SetCachingMode(CachingMode mode);
  void SetHeader(char* header);

  void Log(char c);

  void LogN(char* c, size_t n);

  void Flush();
  void ClearBuffer();
};
}  // namespace robotics::logger
