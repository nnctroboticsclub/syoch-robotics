#pragma once

#include <robotics/controller/packet.hpp>

namespace project {
struct ValueStore {
  struct Config {};

  ValueStore(Config const& config = {}) {}

  bool Pass(controller::RawPacket const& packet) { return true; }
};
}  // namespace project