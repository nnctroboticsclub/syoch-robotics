#pragma once

namespace robotics::network::fep {
enum class ResultType { kOk, kError };

struct DriverResult {
  ResultType type;
  int value;

  bool Failed() const;
};
}  // namespace robotics::network::fep