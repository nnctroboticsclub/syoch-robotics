#pragma once

#include <cstdint>
#include <cstddef>

namespace robotics::network::froute {
struct AddrRecords {
  uint8_t records[32] = {0};
  size_t recorded = 0;

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

  void Remove(uint8_t rec) {
    int remove_to = -1;
    for (size_t i = 0; i < recorded; i++) {
      if (records[i] == rec) {
        remove_to = i;
        break;
      }
    }
    if (remove_to == -1) return;

    for (size_t i = remove_to; i < recorded - 1; i++) {
      records[i] = records[i + 1];
    }
    recorded--;
  }
};
}  // namespace robotics::network::froute