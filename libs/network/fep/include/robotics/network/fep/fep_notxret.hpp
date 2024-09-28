#pragma once

#include <robotics/network/stream.hpp>
#include "fep_raw_driver.hpp"

namespace robotics::network::fep {
class RawFEP_NoTxRet : public Stream<uint8_t, uint8_t> {
  FEP_RawDriver& fep_;

 public:
  RawFEP_NoTxRet(FEP_RawDriver& upper_stream) : fep_(upper_stream) {
    upper_stream.OnReceive([this](uint8_t addr, uint8_t* data, uint32_t len) {
      this->DispatchOnReceive(addr, data, len);
    });
  }

  void Send(uint8_t addr, uint8_t* data, uint32_t len) override {
    fep_.Send(addr, data, len);
  }
};
}  // namespace robotics::network::fep