#pragma once

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <cstring>
#include <cstdio>

#include <functional>

#include "rtos.h"
#include "PinName.h"

#define ENABLE true
#define DISABLE false

#define STM32F446xx 1

namespace mbed {
namespace pseudo {

namespace core_lib {
template <typename T, size_t N>
class CircularBuffer {
 private:
  T buffer[N];
  int head = 0;
  int tail = 0;

 public:
  bool empty() const { return head == tail; }
  bool full() const { return (head + 1) % N == tail; }
  size_t size() const { return (head - tail + N) % N; }
  size_t capacity() const { return N; }
  void push(T value) {
    if (!full()) {
      buffer[head] = value;
      head = (head + 1) % N;
    }
  }
  bool pop(T& value) {
    if (!empty()) {
      value = buffer[tail];
      tail = (tail + 1) % N;
      return true;
    }
    return false;
  }
};

template <typename F>
class Callback : std::function<F> {
 public:
  Callback(F&& f) : std::function<F>(f) {}

  template <typename C, typename R, typename... Args>
  Callback(C* c, R (C::*m)(Args...))
      : std::function<F>([c, m](Args... args) { (c->*m)(args...); }) {}

  using std::function<F>::operator();
};

template <typename C, typename R, typename... Args>
Callback<R(Args...)> callback(C* c, R (C::*m)(Args...)) {
  return Callback<R(Args...)>(c, m);
}
}  // namespace core_lib
using namespace core_lib;

namespace hal {

enum HAL_StatusTypeDef { HAL_OK };

enum CANFilterMode {
  CAN_FILTERMODE_IDMASK,
  CAN_FILTERMODE_IDLIST,
};

enum CANFilterScale { CAN_FILTERSCALE_32BIT };
enum CANFilterFIFOAssignment { CAN_FILTER_FIFO0 };
struct FDCAN_HandleTypeDef {};

struct can_t {
  FDCAN_HandleTypeDef CanHandle;
};
enum CANFormat : uint8_t { CANStandard, CANExtended };
struct CAN_FilterConfTypeDef {
  uint32_t FilterIdHigh;
  uint32_t FilterIdLow;
  uint32_t FilterMaskIdHigh;
  uint32_t FilterMaskIdLow;
  CANFilterFIFOAssignment FilterFIFOAssignment;
  uint32_t FilterNumber;
  CANFilterMode FilterMode;
  CANFilterScale FilterScale;
  bool FilterActivation;
  uint32_t BankNumber;
};

HAL_StatusTypeDef HAL_CAN_ConfigFilter(FDCAN_HandleTypeDef* hcan,
                                       CAN_FilterConfTypeDef* sFilterConfig);
};  // namespace hal
using namespace hal;

struct CANMessage {
  uint32_t id;
  uint8_t data[8];
  uint8_t len;
  hal::CANFormat format;
  bool type;

  CANMessage(uint32_t id = 0, const uint8_t* data = nullptr, uint8_t len = 0,
             hal::CANFormat format = hal::CANStandard, bool type = false);
};

class CAN {
  static int last_tracking_id;
  int tracking_id_;

 protected:
  hal::can_t _can;

 public:
  enum IrqType { RxIrq, TxIrq };

  CAN(PinName rx, PinName tx, int frequency = 1E6);
  virtual void lock();
  virtual void unlock();

  int write(CANMessage msg);
  int read(CANMessage& msg, int handle = 0);
  void reset();

  void attach(Callback<void()> func, IrqType type = RxIrq);

  int rderror();
  int tderror();
};

class I2C {
  static int last_tracking_id;
  int tracking_id_;

 public:
  I2C(PinName sda, PinName scl);
  void write(int address, const char* data, int length, bool repeated = false);
  void read(int address, char* data, int length, bool repeated = false);

  void frequency(int hz);
};

class SPI {
  static int last_tracking_id;
  int tracking_id_;

 public:
  SPI(PinName mosi, PinName miso, PinName sclk, PinName ssel = NC);
  void frequency(int hz);
  void write(const char* data, int length, char* receive, int receive_length);

  void format(int bits, int mode);
};

enum SPI_SR_Flags {
  SPI_SR_TXE = 0x2,
};

struct SPI_TypeDef {
  uint32_t CR1; /*!< SPI control register 1 (not used in I2S mode),      Address
                   offset: 0x00 */
  uint32_t CR2; /*!< SPI control register 2,                             Address
                   offset: 0x04 */
  uint32_t SR;  /*!< SPI status register,                                Address
                   offset: 0x08 */
  uint32_t DR;  /*!< SPI data register,                                  Address
                   offset: 0x0C */
  uint32_t CRCPR;   /*!< SPI CRC polynomial register (not used in I2S mode),
                       Address offset: 0x10 */
  uint32_t RXCRCR;  /*!< SPI RX CRC register (not used in I2S mode),  Address
                       offset: 0x14 */
  uint32_t TXCRCR;  /*!< SPI TX CRC register (not used in I2S mode),  Address
                       offset: 0x18 */
  uint32_t I2SCFGR; /*!< SPI_I2S configuration register, Address offset: 0x1C */
  uint32_t I2SPR;   /*!< SPI_I2S prescaler register,   Address offset: 0x20 */
};

class Timer {
 public:
  Timer();
  void start();
  void reset();
  void stop();
  std::chrono::microseconds elapsed_time();
  int read_ms();
};
class PwmOut {
  PinName pin_;
  int period_;
  int pulsewidth_;

 public:
  PwmOut(PinName pin);
  void period_us(int period);
  void pulsewidth_us(int pulsewidth);
};

class DigitalOut {
  bool value_;
  PinName pin_;

 public:
  DigitalOut(PinName pin);
  void write(bool value);

  operator bool();
  bool operator=(bool value);
};

}  // namespace pseudo
using namespace pseudo;
}  // namespace mbed

using namespace mbed;
using namespace std;