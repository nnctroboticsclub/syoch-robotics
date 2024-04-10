#pragma once

#include "spi.hpp"

#include <memory>
#include <stdexcept>

#include <mbed.h>

namespace robotics::datalink {
class SPI : public ISPI {
  mbed::SPI spi_;

 public:
  SPI(PinName mosi, PinName miso, PinName sclk, PinName ssel, int frequency = 1E6)
      : spi_(mosi, miso, sclk, ssel) {
        spi_.frequency(frequency);
      }

  int Transfer(std::vector<uint8_t> const &tx,
                std::vector<uint8_t> &rx) override {
    if (tx.size() != rx.size()) {
      return -1;
    }

    spi_.write((const char*)tx.data(), (int)tx.size(), (char*)rx.data(), (int)rx.size());

    return tx.size();
  }
};
}  // namespace robotics::datalink