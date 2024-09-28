#pragma once

#include <robotics/logger/logger.hpp>
#include <robotics/network/stream.hpp>
#include <robotics/network/ssp/ssp.hpp>

namespace robotics::network::ssp {
template <typename Context>
class RelayService : public robotics::network::ssp::SSP_Service<Context> {
 public:
  RelayService(robotics::network::Stream<uint8_t, uint8_t>& stream)
      : SSP_Service<Context>(stream, 0x02, "relay.svc.nw",
                             "\x1b[32mRelayService\x1b[m") {
    OnReceive([this, &stream](uint8_t addr, uint8_t* data, size_t len) {
      logger.Info("RX: %d", addr);
      logger.Hex(robotics::logger::core::Level::kInfo, data, len);

      if (len > 0) {
        addr = data[0];
        data += 1;
        len -= 1;
        logger.Info("TX: %d", addr);
        logger.Hex(robotics::logger::core::Level::kInfo, data, len);
        stream.Send(addr, data, len);
      }
    });
  }
};
}  // namespace robotics::network::ssp