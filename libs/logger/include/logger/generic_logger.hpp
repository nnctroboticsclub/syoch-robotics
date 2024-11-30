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

  void RenameTag(const char* new_tag);

  void Supress();
  void Resume();

  void Log(core::Level level, const char* fmt, ...);
  void Info(const char* fmt, ...);
  void Error(const char* fmt, ...);
  void Debug(const char* fmt, ...);
  void Trace(const char* fmt, ...);
  void Verbose(const char* fmt, ...);

  void Hex(core::Level level, const uint8_t* data, size_t length);

  inline void HexInfo(const uint8_t* data, size_t length) {
    Hex(core::Level::kInfo, data, length);
  }
  inline void HexError(const uint8_t* data, size_t length) {
    Hex(core::Level::kError, data, length);
  }
  inline void HexDebug(const uint8_t* data, size_t length) {
    Hex(core::Level::kDebug, data, length);
  }
  inline void HexTrace(const uint8_t* data, size_t length) {
    Hex(core::Level::kTrace, data, length);
  }
  inline void HexVerbose(const uint8_t* data, size_t length) {
    Hex(core::Level::kVerbose, data, length);
  }

  static GenericLogger* GetLogger(const char* id);
};

}  // namespace robotics::logger
