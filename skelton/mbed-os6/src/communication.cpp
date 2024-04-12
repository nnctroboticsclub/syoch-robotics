#include "communication.hpp"

#include <cmath>
#include <mbed-robotics/simple_can.hpp>
#include <ikarashiCAN_mk2.h>

#include "project/identify.h"

void Communication::InitCAN() {
  printf("\e[1;32m|\e[m \e[32m-\e[m Initializing CAN (Com)\n");
  printf("\e[1;32m|\e[m \e[32m|\e[m \e[33m-\e[m Initializing CAN Driver\n");
  can_.Init();
  driving_->Init();
  printf("\e[1;32m|\e[m \e[32m|\e[m \e[33m-\e[m Adding Handlers\n");
  can_.OnMessage(0x61 + project::kCanId, [this](std::vector<uint8_t> data) {  //
    value_store_.Pass(data);
  });
}

Communication::Communication(Config &config)
    : can_(config.can.id,
           std::make_shared<robotics::network::SimpleCAN>(
               config.can.rx, config.can.tx, config.can.freqency)),
      driving_(std::make_unique<project::DrivingCANBus>(new ikarashiCAN_mk2(
          config.driving_can.rx, config.driving_can.tx, 0))),
      value_store_(config.value_store_config) {}

void Communication::SendNonReactiveValues() {
  this->driving_->Tick();
  this->driving_->Send();
}

void Communication::Init() { InitCAN(); }

void Communication::SetStatus(
    robotics::network::DistributedCAN::Statuses status) {
  can_.SetStatus(status);
}

void Communication::AddCAN1Debug() {
  can_.OnRx([](uint32_t id, std::vector<uint8_t> data) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(4) << id << ": ";
    for (auto byte : data) {
      ss << std::setw(2) << std::hex << (int)byte << " ";
    }
    printf("CAN1<-- %s\n", ss.str().c_str());
  });

  can_.OnTx([](uint32_t id, std::vector<uint8_t> data) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(4) << id << ": ";
    for (auto byte : data) {
      ss << std::setw(2) << std::hex << (int)byte << " ";
    }
    printf("CAN1--> %s\n", ss.str().c_str());
  });
}

void Communication::Report() {
  switch () {
    case 0:
    case 1:
    case 2:
    case 3:
      break;
  }
  report_counter = (report_counter + 1) % 4;
}