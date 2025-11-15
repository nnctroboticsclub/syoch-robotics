#pragma once

#include "stream.hpp"

namespace robotics::network {
class IUART : public Stream<uint8_t> {
 public:
  virtual ~IUART() = default;
  virtual void Rebaud(int baud) = 0;
};
}  // namespace robotics::network