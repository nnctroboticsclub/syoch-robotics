#include "panic.hpp"

namespace robotics::system {
void panic(const char* message) {
  printf("Panic: %s\n", message);
  *(volatile int*)0 = 0;

  while (true) {
  }
}
}  // namespace robotics::system