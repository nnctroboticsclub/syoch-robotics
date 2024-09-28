#pragma once

#include "stream.hpp"

namespace robotics::network {
class IUART : public Stream<uint8_t> {
 public:
  virtual void Rebaud(int baud) = 0;
};
}  // namespace robotics::network