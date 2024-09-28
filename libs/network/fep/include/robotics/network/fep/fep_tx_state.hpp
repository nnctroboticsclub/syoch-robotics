#pragma once

namespace robotics::network::fep {
enum class TxState {
  kNoError,
  kTimeout,
  kInvalidResponse,
  kRxOverflow,
  kInvalidCommand
};
}