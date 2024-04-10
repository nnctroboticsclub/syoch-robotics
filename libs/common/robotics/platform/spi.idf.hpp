#pragma once

#include "spi.hpp"

#include <memory>

#include <spi_host_cxx.hpp>

namespace robotics::datalink {
class SPI : public ISPI {
  std::shared_ptr<idf::SPIDevice> spi_;

 public:
  ESP32SPI(std::shared_ptr<idf::SPIDevice> spi) : spi_(spi) {}

  int Transfer(std::vector<uint8_t> const &tx,
               std::vector<uint8_t> &rx) override {
    if (tx.size() != rx.size()) {
      return -1;
    }

    auto desc = spi_->transfer(tx);
    desc.wait();

    if (!desc.valid()) {
      throw std::runtime_error("transfer failed");
    }

    for (size_t i = 0; i < tx.size(); i++) {
      rx[i] = desc.get()[i];
    }

    return tx.size();
  }
};
}  // namespace robotics::datalink