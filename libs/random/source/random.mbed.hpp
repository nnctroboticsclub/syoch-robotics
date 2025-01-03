#include <mbed.h>

#include <robotics/random/random.hpp>

namespace {
mbed::AnalogIn source_{PB_0};
}
void robotics::system::Random::impl::Init() {}
float robotics::system::Random::impl::Entropy() {
  return source_.read();
}