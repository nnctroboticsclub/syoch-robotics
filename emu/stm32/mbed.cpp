#include "mbed.h"

#include <iostream>

#include "../simulation.hpp"

namespace {
simulation::Client client;
auto driver = client.Connect(0);
}  // namespace

using namespace std::chrono_literals;

namespace mbed {
namespace pseudo {

HAL_StatusTypeDef hal::HAL_CAN_ConfigFilter(
    FDCAN_HandleTypeDef* /* hcan */,
    CAN_FilterConfTypeDef* /* sFilterConfig */) {
  return HAL_OK;
}

CANMessage::CANMessage(uint32_t id, const uint8_t* data, uint8_t len,
                       hal::CANFormat format, bool type)
    : id(id), len(len), format(format), type(type) {
  if (data) {
    std::memcpy(this->data, data, len);
  }
}

int CAN::last_tracking_id = 0;

CAN::CAN(PinName rx, PinName tx, int frequency) {
  tracking_id_ = last_tracking_id;

  simulation::Message msg{
      "CCa",
      tracking_id_,
      "init",
      {std::to_string(rx), std::to_string(tx), std::to_string(frequency)}};
  driver->Send(msg);
}
void CAN::lock() {}
void CAN::unlock() {}

int CAN::write(CANMessage msg) {
  simulation::Message sim_msg{"CCa", tracking_id_, "write"};

  sim_msg.args.push_back(std::to_string(msg.id));
  for (int i = 0; i < msg.len; i++) {
    sim_msg.args.push_back(std::to_string(msg.data[i]));
  }

  driver->Send(sim_msg);

  return 1;
}
int CAN::read(CANMessage& msg, int /* handle */) {
  simulation::Message sim_msg{"CCa", tracking_id_, "read"};

  driver->Send(sim_msg);

  simulation::Message response =
      driver->GetMessage("CCa", tracking_id_, "read-responce", 20s);

  msg.id = std::stoi(response.args[0]);
  for (size_t i = 1; i < response.args.size(); i++) {
    msg.data[i - 1] =
        static_cast<uint8_t>(std::stoi(response.args[i])) & 0x000000ff;
  }
  return 0;
}
void CAN::reset() {}

void CAN::attach(Callback<void()> /* func */, IrqType /* type */) {}

int CAN::rderror() { return 0; }
int CAN::tderror() { return 0; }

int I2C::last_tracking_id = 0;
I2C::I2C(PinName sda, PinName scl) {
  tracking_id_ = last_tracking_id;

  simulation::Message msg{
      "OI2", tracking_id_, "init", {std::to_string(sda), std::to_string(scl)}};
  driver->Send(msg);
}
void I2C::write(int address, const char* data, int length, bool repeated) {
  simulation::Message msg{"OI2", tracking_id_, "write"};

  msg.args.push_back(std::to_string(address));
  msg.args.push_back(std::to_string(repeated));
  for (int i = 0; i < length; i++) {
    msg.args.push_back(std::to_string(static_cast<int>(data[i])));
  }

  driver->Send(msg);
}
void I2C::read(int address, char* data, int length, bool repeated) {
  simulation::Message msg{"OI2", tracking_id_, "read"};

  msg.args.push_back(std::to_string(address));
  msg.args.push_back(std::to_string(length));
  msg.args.push_back(std::to_string(repeated));

  driver->Send(msg);

  simulation::Message response =
      driver->GetMessage("OI2", tracking_id_, "read-responce", 20s);

  for (int i = 0; i < length; i++) {
    data[i] = static_cast<uint8_t>(std::stoi(response.args[i])) & 0x000000ff;
  }
}

void I2C::frequency(int /* hz */) {
  // throw simulation::NotImplemenetedError("I2C Frequency");
}

int SPI::last_tracking_id = 0;
SPI::SPI(PinName mosi, PinName miso, PinName sclk) {
  tracking_id_ = last_tracking_id;

  simulation::Message msg{
      "OSp",
      tracking_id_,
      "init",
      {std::to_string(mosi), std::to_string(miso), std::to_string(sclk)}};
  driver->Send(msg);
}
void SPI::frequency(int hz) {
  simulation::Message msg{
      "OSp", tracking_id_, "frequency", {std::to_string(hz)}};
  driver->Send(msg);
}
void SPI::write(const char* data, int length, char* receive,
                int receive_length) {
  simulation::Message msg{"OSp", tracking_id_, "write"};

  for (int i = 0; i < length; i++) {
    msg.args.push_back(std::to_string(static_cast<int>(data[i])));
  }

  driver->Send(msg);

  simulation::Message response =
      driver->GetMessage("OSp", tracking_id_, "write-responce", 20s);

  for (int i = 0; i < receive_length; i++) {
    receive[i] = static_cast<uint8_t>(std::stoi(response.args[i])) & 0x000000ff;
  }
}

void SPI::format(int bits, int mode) {
  simulation::Message msg{"OSp", tracking_id_, "format"};

  msg.args.push_back(std::to_string(bits));
  msg.args.push_back(std::to_string(mode));

  driver->Send(msg);
}

Timer::Timer() {}
void Timer::start() {}
void Timer::reset() { throw simulation::NotImplemenetedError("Timer Reset"); }
std::chrono::microseconds Timer::elapsed_time() {
  throw simulation::NotImplemenetedError("Timer Elapsed Time");
}
int Timer::read_ms() {
  throw simulation::NotImplemenetedError("Timer Read MS");
}

//* PwmOut
PwmOut::PwmOut(PinName pin) : pin_(pin) {
  simulation::Message msg{"OPw", pin, "init", {}};
  driver->Send(msg);
}

void PwmOut::period_us(int period) {
  period_ = period;

  simulation::Message msg{"OPw", pin_, "period", {std::to_string(period)}};
  driver->Send(msg);
}

void PwmOut::pulsewidth_us(int pulsewidth) {
  pulsewidth_ = pulsewidth;

  simulation::Message msg{
      "OPw", pin_, "pulsewidth", {std::to_string(pulsewidth)}};
  driver->Send(msg);
}

//* DigitalOut
DigitalOut::DigitalOut(PinName pin) : pin_(pin) {
  simulation::Message msg{"ODi", pin, "init", {}};
  driver->Send(msg);
}
void DigitalOut::write(bool value) {
  value_ = value;

  simulation::Message msg{"ODi", pin_, "write", {std::to_string(value)}};
  driver->Send(msg);
}

DigitalOut::operator bool() { return value_; }
bool DigitalOut::operator=(bool value) {
  write(value);
  return value;
}

}  // namespace pseudo
using namespace pseudo;
}  // namespace mbed

using namespace mbed;