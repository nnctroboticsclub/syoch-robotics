#pragma once

#include <vector>
#include <memory>

#include <cstdint>

#include "../platform/spi.hpp"

namespace robotics::utils {
class NeoPixelDriver {
 public:
  virtual void SetMaxBytes(int bytes) = 0;

  virtual void Flush() = 0;
  virtual void SetByte(int position, uint8_t byte) = 0;
};

class NeoPixelSPIDriver : public NeoPixelDriver {
  static constexpr int kResetSize = 48 * 2;

  std::shared_ptr<robotics::datalink::ISPI> spi_;

  std::vector<uint8_t> buffer_;

 public:
  NeoPixelSPIDriver(std::shared_ptr<robotics::datalink::ISPI> spi)
      : spi_(spi) {}

  void SetMaxBytes(int bytes) override;
  void Flush() override;
  void SetByte(int position, uint8_t byte) override;
};
}  // namespace robotics::utils