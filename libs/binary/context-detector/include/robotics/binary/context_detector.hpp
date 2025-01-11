#pragma once

namespace robotics::binary {

inline bool InNonWaitableContext() {
#if SR_BCD_PL_MBED
  auto* icsr = reinterpret_cast<volatile uint32_t*>(0xE000ED04);
  return (*icsr & 0x000000ff) != 0;  // VECTACTIVE
#else
  return false;
#endif
}

}  // namespace robotics::binary