#pragma once

#include <vector>
#include <cstdint>

namespace controller {

using RawPacketData = std::vector<uint8_t>;

struct RawPacket {
  uint8_t element_id;
  RawPacketData data;

  RawPacket(RawPacketData const& raw_data) : element_id(raw_data[0]), data{} {
    if (raw_data.size() < 1) {
      return;
    }
    data.reserve(raw_data.size() - 1);
    for (std::size_t i = 1; i < raw_data.size(); i++) {
      data.push_back(raw_data[i]);
    }
  }

  uint8_t operator[](int index) const { return data[index]; }

  std::size_t size() const { return data.size(); }
};

}  // namespace controller