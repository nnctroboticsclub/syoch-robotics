#pragma once

#include <cstdint>
#include <robotics/network/stream.hpp>

#include "checksum.hpp"

#include <robotics/utils/no_mutex_lifo.hpp>

#include <robotics/network/fep/fep_raw_driver.hpp>
#include <robotics/network/fep/fep_tx_state.hpp>

namespace robotics::network {
namespace rep {
struct REPTxPacket {
  uint8_t addr;
  std::array<uint8_t, 32> buffer;
  uint32_t length;
};

class ReliableFEPProtocol : public Stream<uint8_t, uint8_t> {
  Stream<uint8_t, uint8_t, fep::TxState>& driver_;

  Checksum rx_cs_calculator;
  Checksum tx_cs_calculator;

  bool in_isr = false;

  std::array<uint8_t, 32> tx_buffer_{};

  robotics::utils::NoMutexLIFO<REPTxPacket, 4> tx_queue;

  void _Send(REPTxPacket& packet);

 public:
  explicit ReliableFEPProtocol(fep::FEP_RawDriver& driver);

  void Send(uint8_t address, uint8_t* data, uint32_t length) override;
};

}  // namespace rep
using ReliableFEPProtocol = rep::ReliableFEPProtocol;
}  // namespace robotics::network