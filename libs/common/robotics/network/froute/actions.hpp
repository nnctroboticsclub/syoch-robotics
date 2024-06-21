#pragma once

#include <cstdint>

namespace robotics::network::froute {
struct Action {
  enum class Type { kFindNewHopTo, kReportDiedPacket, kAdvertiseSelf };
  Type type;

  uint8_t report_to;

  static Action FindNewHopTo() { return {Type::kFindNewHopTo, 0}; }

  static Action ReportDiedPacket(uint8_t report_to) {
    return {Type::kReportDiedPacket, report_to};
  }

  static Action AdvertiseSelf(uint8_t report_to) {
    return {Type::kAdvertiseSelf, report_to};
  }
};
}  // namespace robotics::network::froute