#include <can_api.h>
#include <interfaces/InterfaceCAN.h>
#include <cstddef>
#include <robotics/network/simple_can.hpp>

using IdleCallback = robotics::network::CANBase::IdleCallback;
using RxCallback = robotics::network::CANBase::RxCallback;
using TxCallback = robotics::network::CANBase::TxCallback;

class CANHack_t : private mbed::CAN {
 public:
  static size_t kOffset_can_t;
};
static_assert(sizeof(CANHack_t) == sizeof(mbed::CAN),
              "CANHack_t should be the same size as mbed::CAN");
size_t CANHack_t::kOffset_can_t = offsetof(CANHack_t, _can);

size_t kOffset_can_t = CANHack_t::kOffset_can_t;

namespace robotics::network {
class SimpleCAN::Impl {
  void ThreadMain() {
    this->thread_running_ = true;
    CANMessage msg;
    while (true) {
      if (this->thread_stop_) {
        this->thread_running_ = false;
        return;
      }

      if (can_.rderror() || can_.tderror()) {
        can_.reset();
        ThisThread::sleep_for(10ms);
      }

      if (!this->idle_callbacks_.empty()) {
        for (auto cb : this->idle_callbacks_) {
          cb();
        }
      }
    }
  }

 public:
  Impl(PinName rx, PinName tx, int frequency) : can_(rx, tx, frequency) {}

  ~Impl() {
    printf("~SimpleCAN()\n");
    if (thread_) {
      thread_stop_ = true;
      while (thread_running_) {
        ThisThread::sleep_for(1ms);
      }
      delete thread_;
    }
  }

  int Send(uint32_t id, std::vector<uint8_t> const& data) {
    for (auto const& cb : tx_callbacks_) {
      cb(id, data);
    }

    CANMessage msg;
    msg.format =
        is_can_extended_ ? CANFormat::CANExtended : CANFormat::CANStandard;
    msg.id = id;
    msg.len = data.size();
    std::copy(data.begin(), data.end(), msg.data);
    return can_.write(msg);
  }

  void SetCANExtended(bool is_can_extended) {
    is_can_extended_ = is_can_extended;
  }

  void Init() {
    thread_ = new Thread(osPriorityNormal, 0x800);
    thread_->start([this] { ThreadMain(); });

    can_.attach(
        [this]() {
          const auto ptr_int = reinterpret_cast<uintptr_t>(this);
          const auto ptr_can =
              reinterpret_cast<CANHack_t*>(ptr_int + kOffset_can_t);
          // Assumption: The memory layout of CANHack_t and mbed::CAN is identical,
          // and kOffset_can_t correctly represents the offset of the _can member.
          // This reinterpret_cast relies on these assumptions being valid.
          // To safeguard, ensure that the static_assert above verifies the size equivalence.
          // Additionally, consider runtime checks if the platform or compiler changes.
          const auto can = reinterpret_cast<can_t*>(ptr_can);

          CANMessage msg;
          // Call LL API directly to avoid requiring a mutex for ISR context restrictions
          can_read(can, &msg, 0);

          std::copy(msg.data, msg.data + msg.len, buffer_.begin());

          for (auto const& cb : rx_callbacks_) {
            cb(msg.id, buffer_);
          }
        },
        mbed::CAN::RxIrq);

    while (!thread_running_) {
      ThisThread::sleep_for(1ms);
    }
  }

  void OnRx(RxCallback cb) { rx_callbacks_.emplace_back(cb); }

  void OnTx(TxCallback cb) { tx_callbacks_.emplace_back(cb); }

  void OnIdle(IdleCallback cb) { idle_callbacks_.emplace_back(cb); }

 private:
  CAN can_;
  bool is_can_extended_ = false;

  bool thread_stop_ = false;
  bool thread_running_ = false;

  std::vector<RxCallback> rx_callbacks_;
  std::vector<TxCallback> tx_callbacks_;
  std::vector<IdleCallback> idle_callbacks_;

  Thread* thread_ = nullptr;
  std::vector<uint8_t> buffer_ = std::vector<uint8_t>(8);
};

SimpleCAN::SimpleCAN(PinName rx, PinName tx, int frequency) {
  impl_ = new Impl(rx, tx, frequency);
}
SimpleCAN::~SimpleCAN() {
  delete impl_;
}

int SimpleCAN::Send(uint32_t id, std::vector<uint8_t> const& data) {
  return impl_->Send(id, data);
}
void SimpleCAN::SetCANExtended(bool is_can_extended) {
  impl_->SetCANExtended(is_can_extended);
}

void SimpleCAN::Init() {
  impl_->Init();
}

void SimpleCAN::OnRx(RxCallback cb) {
  impl_->OnRx(cb);
}
void SimpleCAN::OnTx(TxCallback cb) {
  impl_->OnTx(cb);
}
void SimpleCAN::OnIdle(IdleCallback cb) {
  impl_->OnIdle(cb);
}

}  // namespace robotics::network