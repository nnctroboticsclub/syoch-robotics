#include <esp_random.h>

#include <robotics/random/random.hpp>

float robotics::system::Random::Entropy() {
  return esp_random() / 65536 / 65536.0f;
}
