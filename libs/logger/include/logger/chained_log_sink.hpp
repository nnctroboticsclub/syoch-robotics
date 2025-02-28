#pragma once

#include "logger/log_sink.hpp"

namespace robotics::logger {
class ChainedLogger final : public robotics::logger::LogSink {
  robotics::logger::LogSink* sink1;
  robotics::logger::LogSink* sink2;

 public:
  ChainedLogger(robotics::logger::LogSink* sink1,
                robotics::logger::LogSink* sink2)
      : sink1(sink1), sink2(sink2) {}

  ~ChainedLogger() override = default;

  virtual void Log(robotics::logger::core::Level level, const char* tag,
                   const char* msg) final {
    sink1->Log(level, tag, msg);
    sink2->Log(level, tag, msg);
  }
};
}  // namespace robotics::logger