#pragma once

#include <robotics/random/random.hpp>

#include <random>

namespace {
std::random_device source_;
}

void robotics::system::Random::impl::Init() {}

float robotics::system::Random::impl::Entropy() {
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);
  return dist(source_);
}