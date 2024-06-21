#pragma once

#include <cstdint>
#include <cstddef>

namespace robotics::network::froute {
struct AddrRecords {
  uint8_t records[32];
  size_t recorded;

  bool Recorded(uint8_t rec) {
    for (size_t i = 0; i < recorded; i++) {
      if (records[i] == rec) {
        return true;
      }
    }

    return false;
  }

  void Add(uint8_t rec) {
    if (Recorded(rec)) return;

    records[recorded++] = rec;
  }
};
}  // namespace robotics::network::froute