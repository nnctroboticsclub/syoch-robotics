#pragma once

#include <robotics/network/dcan.hpp>

#include "project/value_stores.hpp"
#include "project/driving_can.hpp"

class Communication {
 public:
  struct Config {
    struct {
      int id;
      int freqency;

      PinName rx, tx;
    } can;

    struct {
      PinName rx, tx;
    } driving_can;

    struct {
      PinName sda;
      PinName scl;
    } i2c;

    project::ValueStore::Config value_store_config;
  };

 public:
  robotics::network::DistributedCAN can_;
  std::unique_ptr<project::DrivingCANBus> driving_;

  project::ValueStore value_store_;

 private:
  int report_counter = 0;
  void InitCAN();

 public:
  Communication(Config &config);

  void SendNonReactiveValues();
  void Init();
  void SetStatus(robotics::network::DistributedCAN::Statuses status);
  void AddCAN1Debug();
  void Report();
};
