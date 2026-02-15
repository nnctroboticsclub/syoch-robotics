#pragma once

#include <stddef.h>
#include <Nano/span.hpp>

#include "level.hpp"

namespace robotics::logger::core {

struct LogLine {
  Level level;
  Nano::collection::Span<const char> tag;
  Nano::collection::Span<const char> msg;

  LogLine() = default;

  LogLine(Nano::collection::Span<const char> tag,
          Nano::collection::Span<const char> msg,
          core::Level level = core::Level::kInfo);
};

}  // namespace robotics::logger::core