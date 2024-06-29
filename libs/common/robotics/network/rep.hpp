#pragma once

#include <cstdint>

#include "checksum.hpp"
#include "stream.hpp"

#include "../utils/no_mutex_lifo.hpp"

#include "fep/fep_tx_state.hpp"
#include "fep/fep_raw_driver.hpp"

namespace robotics::network {
namespace rep {
struct REPTxPacket {
  uint8_t addr;
  uint8_t buffer[32];
  uint32_t length;
};

class ReliableFEPProtocol : public Stream<uint8_t, uint8_t> {
  Stream<uint8_t, uint8_t, fep::TxState>& driver_;

  Checksum rx_cs_calculator;
  Checksum tx_cs_calculator;

  bool in_isr = false;

  uint8_t tx_buffer_[32] = {};

  robotics::utils::NoMutexLIFO<REPTxPacket, 4> tx_queue;

  void _Send(REPTxPacket& packet);

 public:
  ReliableFEPProtocol(FEP_RawDriver& driver);

  void Send(uint8_t address, uint8_t* data, uint32_t length) override;
};

}  // namespace rep
using ReliableFEPProtocol = rep::ReliableFEPProtocol;
}  // namespace robotics::network