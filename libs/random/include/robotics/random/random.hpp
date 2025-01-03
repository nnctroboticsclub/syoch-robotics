#pragma once

#include <cmath>
#include <cstdint>

namespace robotics::system {
namespace Random {

namespace impl {
void Init();
float Entropy();
}  // namespace impl

void Init();
uint8_t GetByte();
};  // namespace Random
}  // namespace robotics::system
