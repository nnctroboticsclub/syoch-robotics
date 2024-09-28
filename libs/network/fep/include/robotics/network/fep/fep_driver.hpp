#pragma once

#include <unordered_map>

#include <robotics/driver/dout.hpp>
#include <robotics/network/iuart.hpp>
#include <logger/logger.hpp>

#include "fep_raw_driver.hpp"
#include "fep_notxret.hpp"

namespace robotics::network::fep {
// FEP Baudrate (Raw value) the value presented FEP's REG20 parameter
enum class FEPBaudrateValue {
  k9600 = 0,
  k19200 = 1,
  k38400 = 2,
  k115200 = 3,
};

class FEPBaudrate {
  FEPBaudrateValue value_;

 public:
  FEPBaudrate(FEPBaudrateValue value) : value_(value) {}

  int GetBaudrate() {
    switch (value_) {
      case FEPBaudrateValue::k9600:
        return 9600;
      case FEPBaudrateValue::k19200:
        return 19200;
      case FEPBaudrateValue::k38400:
        return 38400;
      case FEPBaudrateValue::k115200:
        return 115200;
      default:
        return 9600;
    }
  }

  int GetBits() { return (int)value_; }
};

class FEPDriver {
  static inline robotics::logger::Logger logger{"fep.nw",
                                                "\x1b[1;4;32mFEPDriver\x1b[m"};
  robotics::network::IUART *stream_;

  robotics::driver::IDout *fep_rst_;
  robotics::driver::IDout *fep_ini_;

  robotics::network::FEP_RawDriver fep_;

  std::unordered_map<uint8_t, uint8_t> configured_registers_ = {};
  FEPBaudrate configured_baudrate_ = FEPBaudrateValue::k115200;

  [[nodiscard]]
  robotics::types::Result<bool, robotics::network::fep::DriverError>
  IsRegisterConfigured(uint8_t addr) {
    if (configured_registers_.find(addr) == configured_registers_.end()) {
      return false;
    }

    auto expected = configured_registers_[addr];

    auto result = fep_.GetRegister(addr);
    if (!result.IsOk()) {
      logger.Error("Failed to get FEP Register: %s",
                   result.UnwrapError().c_str());
      return result.UnwrapError();
    }
    auto actual = result.Unwrap();

    if (expected != actual) {
      return false;
    }

    return true;
  }

  [[nodiscard]]
  robotics::types::Result<void, robotics::network::fep::DriverError>
  ConfigureRegisters() {
    bool any_register_changed = false;
    for (auto &[addr, value] : configured_registers_) {
      bool configured;
      {
        auto result = IsRegisterConfigured(addr);
        if (!result.IsOk()) {
          return result.UnwrapError();
        }

        configured = result.Unwrap();
      }

      if (configured) {
        logger.Info("FEP Register Already Configured: %d: %d", addr, value);
        continue;
      }

      any_register_changed = true;

      auto result = fep_.SetRegister(addr, value);
      if (!result.IsOk()) {
        logger.Error("Failed to set FEP Register: %s",
                     result.UnwrapError().c_str());
        return result.UnwrapError();
      }

      logger.Info("FEP Register Configured: %d: %d", addr, value);
    }

    if (any_register_changed) {
      fep_.Reset();
    }

    return robotics::types::Result<void, robotics::network::fep::DriverError>();
  }

  bool CheckConnectability() {
    auto result = this->fep_.Version(100ms);
    if (!result.IsOk()) {
      logger.Error("Failed to get FEP Version: %s",
                   result.UnwrapError().c_str());

      return false;
    }

    return true;
  }

  void AdjustBaudrateToRemote(bool allow_failed = true) {
    if (CheckConnectability()) {
      logger.Info("FEP Baudrate Detected: %d", 115200);
      return;
    }

    FEPBaudrate candidates[] = {
        // Fastest baudrate is might be the most used baudrate
        FEPBaudrate(FEPBaudrateValue::k115200),

        // Default baudrate
        FEPBaudrate(FEPBaudrateValue::k9600),

        // Other baudrates, but in the order of the speed
        FEPBaudrate(FEPBaudrateValue::k38400),
        FEPBaudrate(FEPBaudrateValue::k19200),
    };

    for (auto &baudrate : candidates) {
      logger.Info("Trying FEP Baudrate: %d", baudrate.GetBaudrate());
      this->stream_->Rebaud(baudrate.GetBaudrate());

      if (CheckConnectability()) {
        logger.Info("FEP Baudrate Detected: %d", baudrate.GetBaudrate());
        return;
      }
    }

    if (allow_failed) {
      robotics::system::SleepFor(1000ms);
      ResetRegistersHW();
      this->stream_->Rebaud(candidates[0].GetBaudrate());
    } else {
      logger.Error("Failed to detect FEP Baudrate");
      while (1) robotics::system::SleepFor(1000ms);
    }
  }

  void ChangeBaudRate(FEPBaudrate baudrate) {
    uint8_t reg20;

    {
      auto result = this->fep_.GetRegister(20);
      if (!result.IsOk()) {
        logger.Error("Failed to get FEP Baudrate: %s",
                     result.UnwrapError().c_str());
        return;
      }
      reg20 = result.Unwrap();
    }

    // Replace the baudrate bits(LSB 2 bits)
    auto new_reg20 = (reg20 & 0xFC) | baudrate.GetBits();

    if (new_reg20 == reg20) {
      logger.Info("FEP Baudrate Unchanged: %d", baudrate.GetBaudrate());
      return;
    }

    {
      auto result = this->fep_.SetRegister(20, new_reg20);
      if (!result.IsOk()) {
        logger.Error("Failed to set FEP Baudrate: %s",
                     result.UnwrapError().c_str());
        return;
      }
    }

    this->fep_.ResetNoResult();

    logger.Info("FEP Baudrate Changing to: %d", baudrate.GetBaudrate());
    this->stream_->Rebaud(baudrate.GetBaudrate());
  }

 public:
  FEPDriver(robotics::network::IUART &stream, robotics::driver::IDout &fep_rst,
            robotics::driver::IDout &fep_ini)
      : stream_(&stream),
        fep_rst_(&fep_rst),
        fep_ini_(&fep_ini),
        fep_(*stream_) {
    fep_rst_->Write(true);
    fep_ini_->Write(true);
  }

  robotics::network::fep::FEP_RawDriver &GetFEP() { return fep_; }

  RawFEP_NoTxRet GetFEP_NoTxRet() { return RawFEP_NoTxRet(fep_); }

  void ResetHW() {
    logger.Info("Resetting FEP Hardware after 2s");
    robotics::system::SleepFor(2s);
    logger.Info("Resetting FEP Hardware");
    fep_rst_->Write(false);
    robotics::system::SleepFor(100ms);

    fep_rst_->Write(true);
  }

  void ResetRegistersHW() {
    logger.Info("Resetting FEP Registers");
    fep_ini_->Write(false);
    robotics::system::SleepFor(100ms);

    ResetHW();
    robotics::system::SleepFor(100ms);

    fep_ini_->Write(true);

    this->stream_->Rebaud(9600);
  }

  [[nodiscard]]
  robotics::types::Result<void, robotics::network::fep::DriverError> Init() {
    fep_.SetDispatchRX(false);
    AdjustBaudrateToRemote();

    ChangeBaudRate(configured_baudrate_);
    AdjustBaudrateToRemote();

    auto result = ConfigureRegisters();
    if (!result.IsOk()) return result.UnwrapError();

    fep_.SetDispatchRX(true);

    return robotics::types::Result<void, robotics::network::fep::DriverError>();
  }

  void AddConfiguredRegister(uint8_t addr, uint8_t value) {
    configured_registers_[addr] = value;
  }

  void ConfigureBaudrate(FEPBaudrate baudrate) {
    configured_baudrate_ = baudrate;
  }
};
}  // namespace robotics::network::fep