#pragma once

#include <cstdint>

namespace robotics::network::fep {
struct RxProcessorPacket {
  enum class Type : uint8_t {
    kData,
    kResult,
    kLine,
    kEnterProcessing,
    kExitProcessing,
  };

  Type type;

  union {
    FEPPacket data;
    DriverResult result;
    FEPRawLine line;
  };

  static RxProcessorPacket Data(FEPPacket fep_packet) {
    RxProcessorPacket packet;
    packet.type = Type::kData;
    packet.data = fep_packet;
    return packet;
  }

  static RxProcessorPacket Result(DriverResult result) {
    RxProcessorPacket packet;
    packet.type = Type::kResult;
    packet.result = result;
    return packet;
  }

  static RxProcessorPacket Line(char const* line) {
    RxProcessorPacket packet;
    packet.type = Type::kLine;
    strncpy(packet.line.line, line, 64);
    return packet;
  }

  static RxProcessorPacket EnterProcessing() {
    RxProcessorPacket packet;
    packet.type = Type::kEnterProcessing;
    return packet;
  }

  static RxProcessorPacket ExitProcessing() {
    RxProcessorPacket packet;
    packet.type = Type::kExitProcessing;
    return packet;
  };
};
}  // namespace robotics::network::fep