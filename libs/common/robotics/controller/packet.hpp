#pragma once

#include <vector>
#include <cstdint>

namespace controller {

struct RawPacket {
  uint8_t element_id;
  uint8_t *data_;
  size_t size_;

  RawPacket(uint8_t *data, size_t length)
      : element_id(data[0]), data_(data + 1), size_(length - 1) {}

  uint8_t operator[](int index) const { return data_[index]; }

  std::size_t size() const { return size_; }
};

}  // namespace controller