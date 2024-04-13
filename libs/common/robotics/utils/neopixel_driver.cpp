#include "neopixel_driver.hpp"

namespace robotics::utils {
void NeoPixelSPIDriver::SetMaxBytes(int bytes) {
  buffer_.resize(kResetSize + bytes * 8 / 2);

  for (int i = 0; i < buffer_.size(); i++) {
    buffer_[i] = 0;
  }
}

void NeoPixelSPIDriver::Flush() {
  std::vector<uint8_t> rx(buffer_.size());
  spi_->Transfer(buffer_, rx);
}

void NeoPixelSPIDriver::SetByte(int position, uint8_t byte) {
  const int byte_index = kResetSize + position * 4;

  buffer_[byte_index + 0] =
      ((byte >> 7) & 1 ? 0xE : 0x8) << 4 | ((byte >> 6) & 1 ? 0xE : 0x8);
  buffer_[byte_index + 1] =
      ((byte >> 5) & 1 ? 0xE : 0x8) << 4 | ((byte >> 4) & 1 ? 0xE : 0x8);
  buffer_[byte_index + 2] =
      ((byte >> 3) & 1 ? 0xE : 0x8) << 4 | ((byte >> 2) & 1 ? 0xE : 0x8);
  buffer_[byte_index + 3] =
      ((byte >> 1) & 1 ? 0xE : 0x8) << 4 | ((byte >> 0) & 1 ? 0xE : 0x8);
}
}  // namespace robotics::utils