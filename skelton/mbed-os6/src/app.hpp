#pragma once

#include <atomic>

#include <mbed.h>

#include <robotics/utils/emc.hpp>
#include <robotics/node/digital_out.hpp>
#include <robotics/utils/neopixel.hpp>

#include "communication.hpp"
#include "components/upper.hpp"
#include "components/swerve.hpp"

#include <robotics/utils/neopixel.hpp>

class App {
  class Impl;

 public:
  struct Config {
    Communication::Config com;

    robotics::component::Swerve::Config swerve_config;

    bool can1_debug;
  };

  Impl* impl;

 public:
  App(Config& config);

  void Init();
};