#pragma once

#include <cstdio>

namespace robotics::system {
[[noreturn]] void panic(const char* message);
}  // namespace robotics::system