#include <can_api.h>
#include <mbed.h>
#include <cstddef>
#include <robotics/network/simple_can.hpp>
#include "robotics/utils/no_mutex_lifo.hpp"

using IdleCallback = robotics::network::CANBase::IdleCallback;
using RxCallback = robotics::network::CANBase::RxCallback;
using TxCallback = robotics::network::CANBase::TxCallback;

class CANHack_t : private mbed::CAN {
 public:
  static size_t kOffset_can_t;
};
size_t CANHack_t::kOffset_can_t = offsetof(CANHack_t, _can);

static_assert(sizeof(CANHack_t) == sizeof(mbed::CAN));

static auto GetCANAPI(mbed::CAN& can) -> can_t* {
  auto ptr_can = reinterpret_cast<uintptr_t>(&can) + CANHack_t::kOffset_can_t;
  return reinterpret_cast<can_t*>(ptr_can);
}

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
        for (auto const& cb : this->idle_callbacks_) {
          cb();
        }
      }

      if (!this->tx_queue.Empty()) {
        auto [id, data] = this->tx_queue.Pop();
        this->Send(id, data);
      }
    }
  }

  [[nodiscard]] auto GetCANAPI() { return ::GetCANAPI(this->can_); }
  [[nodiscard]] auto GetHALCAN() { return GetCANAPI()->CanHandle; }

 public:
  Impl(PinName rx, PinName tx, int frequency, bool is_can_extended = false)
      : can_(rx, tx, frequency), is_can_extended_(is_can_extended) {}

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
    CANMessage msg;
    msg.format =
        is_can_extended_ ? CANFormat::CANExtended : CANFormat::CANStandard;
    msg.id = id;
    msg.len = data.size();
    std::copy(data.begin(), data.end(), msg.data);

    if (can_.write(msg) == 0) {  // failed
      tx_queue.Push({id, data});
      return -1;
    }

    for (auto const& cb : tx_callbacks_) {
      cb(id, data);
    }
    return 0;
  }

  void SetCANExtended(bool is_can_extended) {
    is_can_extended_ = is_can_extended;
  }

  void Init() {
    tx_queue.Clear();

    thread_ = new Thread(osPriorityNormal, 0x800);
    thread_->start([this] { ThreadMain(); });

    can_.attach(
        [this]() {
          CANMessage msg;
          // Call LL API directly to avoid requiring a mutex for ISR context restrictions
          can_read(GetCANAPI(), &msg, 0);

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

  utils::NoMutexLIFO<std::pair<uint32_t, std::vector<uint8_t>>, 16> tx_queue;

  Thread* thread_ = nullptr;
  std::vector<uint8_t> buffer_ = std::vector<uint8_t>(8);
};

SimpleCAN::SimpleCAN(PinName rx, PinName tx, int frequency,
                     bool is_can_extended) {
  impl_ = new Impl(rx, tx, frequency, is_can_extended);
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