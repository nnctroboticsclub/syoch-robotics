#pragma once

#include <cstdio>

namespace robotics::system {
[[noreturn]] void panic(const char* message) {
  printf("Panic: %s\n", message);
  *(volatile int*)0 = 0;

  while (true) {
  }
}
}  // namespace robotics::system