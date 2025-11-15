#include <robotics/platform/panic.hpp>

namespace robotics::system {
void panic(const char* message) {
  printf("Panic: %s\n", message);

  while (true)
    ;
}
}  // namespace robotics::system