#pragma once

#include <NanoHW/policies.hpp>
#include <concepts>
#include <memory>
#include <vector>

#include <cstdint>

#include <NanoHW/spi.hpp>

namespace robotics::utils {
template <typename T>
concept NeoPixelDriver =
    requires(int max_bytes, int position, uint8_t byte, T v) {
  {v.SetMaxBytes(max_bytes)}->std::same_as<void>;
  {v.Flush()}->std::same_as<void>;
  {v.SetByte(position, byte)}->std::same_as<void>;
};

struct NeoPixelSPIHandler {
 public:
  using OnTransfer =
      nano_hw::Direct<[](void* instance, std::vector<uint8_t> const& tx,
                         std::vector<uint8_t> const& rx) {
      }>;
};
static_assert(nano_hw::spi::SPIConfig<NeoPixelSPIHandler>);

template <template <nano_hw::spi::SPIConfig> typename SPI>
requires nano_hw::spi::SPI<SPI> class NeoPixelSPIDriver {
  static constexpr int kResetSize = 48 * 2;

  SPI<NeoPixelSPIHandler> spi_;

  std::vector<uint8_t> buffer_;

 public:
  template <typename... Args>
  NeoPixelSPIDriver(Args... args) : spi_{args...} {}

  void SetMaxBytes(int bytes) {
    buffer_.resize(kResetSize + bytes * 8 / 2);

    for (size_t i = 0; i < buffer_.size(); i++) {
      buffer_[i] = 0;
    }
  }
  void Flush() {
    static std::vector<uint8_t> rx;

    rx.resize(buffer_.size());
    spi_.Transfer(buffer_, rx);
  }
  void SetByte(int position, uint8_t byte) {
    const int byte_index = kResetSize + position * 4;

    if (buffer_.size() <= (unsigned int)(byte_index + 3)) {
      return;
    }

    buffer_[byte_index + 0] =
        ((byte >> 7) & 1 ? 0xE : 0x8) << 4 | ((byte >> 6) & 1 ? 0xE : 0x8);
    buffer_[byte_index + 1] =
        ((byte >> 5) & 1 ? 0xE : 0x8) << 4 | ((byte >> 4) & 1 ? 0xE : 0x8);
    buffer_[byte_index + 2] =
        ((byte >> 3) & 1 ? 0xE : 0x8) << 4 | ((byte >> 2) & 1 ? 0xE : 0x8);
    buffer_[byte_index + 3] =
        ((byte >> 1) & 1 ? 0xE : 0x8) << 4 | ((byte >> 0) & 1 ? 0xE : 0x8);
  }
};

static_assert(NeoPixelDriver<NeoPixelSPIDriver<nano_hw::spi::DynSPI>>);
}  // namespace robotics::utils