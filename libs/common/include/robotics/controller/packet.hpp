#pragma once

#include <cstddef>
#include <cstdint>

namespace controller {

struct RawPacket {
  uint8_t element_id;
  uint8_t *data_;
  std::size_t size_;

  RawPacket(uint8_t *data, std::size_t length)
      : element_id(data[0]), data_(data + 1), size_(length - 1) {}

  uint8_t operator[](int index) const { return data_[index]; }

  [[nodiscard]] std::size_t size() const { return size_; }
};

}  // namespace controller