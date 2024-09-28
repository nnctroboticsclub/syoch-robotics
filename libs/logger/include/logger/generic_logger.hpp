#pragma once

#include <stdarg.h>
#include "./logger-core.hpp"

namespace robotics::logger {

class GenericLogger {
  const char* id;
  const char* tag;

  bool supressed = false;

 private:
  void _Log(core::Level level, const char* fmt, va_list args);
  void _LogHex(core::Level level, const uint8_t* buf, size_t size);

 public:
  GenericLogger(const char* id, const char* tag);

  void Supress();
  void Resume();

  void Log(core::Level level, const char* fmt, ...);
  void Info(const char* fmt, ...);
  void Error(const char* fmt, ...);
  void Debug(const char* fmt, ...);
  void Trace(const char* fmt, ...);
  void Verbose(const char* fmt, ...);

  void Hex(core::Level level, const uint8_t* data, size_t length);

  static GenericLogger* GetLogger(const char* id);
};

}  // namespace robotics::logger
