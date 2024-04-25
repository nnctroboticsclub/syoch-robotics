#pragma once

#include <cstdint>

#include <array>
#include <memory>

#include "../network/dcan.hpp"

namespace robotics::node {

class NodeInspector {
  public:
  static std::shared_ptr<network::DistributedCAN> can;

  private:
  static std::uint16_t latest_id_;

  uint16_t type_;
  uint16_t id_;

 public:
  NodeInspector(std::uint16_t type) : type_(type), id_(latest_id_++) {}

  void Update(std::array<uint8_t, 4> data) {
    if (can == nullptr) {
      return;
    }

    std::array<uint8_t, 8> payload_array;
    payload_array[0] = type_ >> 8;
    payload_array[1] = type_ & 0xFF;
    payload_array[2] = id_ >> 8;
    payload_array[3] = id_ & 0xFF;
    payload_array[4] = data[0];
    payload_array[5] = data[1];
    payload_array[6] = data[2];
    payload_array[7] = data[3];

    std::vector<uint8_t> payload(payload_array.begin(), payload_array.end());

    uint16_t message_id = 0x4d0 + can->GetCanId();

    can->Send(message_id, payload);

  }
};

}  // namespace robotics::node