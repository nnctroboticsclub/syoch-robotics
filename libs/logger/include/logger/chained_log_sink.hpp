#pragma once

#include "logger/log_sink.hpp"

namespace robotics::logger {
class ChainedLogger final : public robotics::logger::LogSink {
  robotics::logger::LogSink* sink1;
  robotics::logger::LogSink* sink2;

  bool sink2_is_active = true;

 public:
  ChainedLogger(robotics::logger::LogSink* sink1,
                robotics::logger::LogSink* sink2)
      : sink1(sink1), sink2(sink2) {}

  [[nodiscard]] bool Sink2Active() const { return sink2_is_active; }
  void Sink2Active(bool is_active) { sink2_is_active = is_active; }

  ~ChainedLogger() override = default;

  void Log(robotics::logger::core::Level level, const char* tag,
           const char* msg) final {
    sink1->Log(level, tag, msg);
    if (sink2_is_active) {
      sink2->Log(level, tag, msg);
    }
  }
};
}  // namespace robotics::logger