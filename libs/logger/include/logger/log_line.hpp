#pragma once

#include <stddef.h>
#include "robotics/utils/span.hpp"

#include "level.hpp"

namespace robotics::logger::core {

struct LogLine {
  Level level;
  utils::Span<const char> tag;
  utils::Span<const char> msg;

  LogLine() = default;

  LogLine(utils::Span<const char> tag, utils::Span<const char> msg,
          core::Level level = core::Level::kInfo);
};

}  // namespace robotics::logger::core