#include <mbed.h>

#include <robotics/random/random.hpp>

namespace {
mbed::AnalogIn source_{PC_0};
}

float robotics::system::Random::Entropy() { return source_.read(); }