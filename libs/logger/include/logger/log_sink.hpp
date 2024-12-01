#pragma once

#include "./logger-core.hpp"

namespace robotics::logger {
class LogSink {
 public:
  virtual ~LogSink() = default;

  virtual void Log(core::Level level, const char* tag, const char* msg) = 0;
};
}