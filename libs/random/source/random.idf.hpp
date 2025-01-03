#include <esp_random.h>

#include <robotics/random/random.hpp>

void robotics::system::Random::impl::Init() {}

float robotics::system::Random::impl::Entropy() {
  return esp_random() / 65536 / 65536.0f;
}
