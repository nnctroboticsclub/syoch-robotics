#include <robotics/network/simple_can.hpp>

namespace robotics::network {
void SimpleCAN::ThreadMain() {
  CANMessage msg;
  while (1) {
    if (can_.rderror() || can_.tderror()) {
      can_.reset();
      ThisThread::sleep_for(10ms);
    }

    if (can_.read(msg)) {
      for (auto const &cb : rx_callbacks_) {
        cb(msg.id, std::vector<uint8_t>(msg.data, msg.data + msg.len));
      }
    }

    if (!this->idle_callbacks_.empty()) {
      for (auto cb : this->idle_callbacks_) {
        cb();
      }
    }
  }
}

SimpleCAN::SimpleCAN(PinName rx, PinName tx, int freqency)
    : can_(rx, tx, freqency), freqency_(freqency) {}

void SimpleCAN::Init() {
  thread_ = new Thread(osPriorityNormal, 1024 * 4);
  thread_->start(callback(this, &SimpleCAN::ThreadMain));
}

void SimpleCAN::OnRx(RxCallback cb) { rx_callbacks_.emplace_back(cb); }

void SimpleCAN::OnTx(TxCallback cb) { tx_callbacks_.emplace_back(cb); }

void SimpleCAN::OnIdle(IdleCallback cb) { idle_callbacks_.emplace_back(cb); }
}  // namespace robotics::network