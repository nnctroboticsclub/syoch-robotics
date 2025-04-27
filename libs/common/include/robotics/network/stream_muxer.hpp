#pragma once

#include <concepts>
#include <cstdio>
namespace robotics::network {
template <typename S1, typename S2>
requires(std::same_as<typename S1::DataType,
                      typename S2::DataType>) class StreamMuxer final
    : public S1::StreamType {
 public:
  StreamMuxer(S1& stream1, S2& stream2)
      : stream1_(stream1), stream2_(stream2) {}

  void Init() {
    stream1_.OnReceive(
        [this](S1::DataType& data) { this->DispatchOnReceive(data); });

    stream2_.OnReceive(
        [this](S2::DataType& data) { this->DispatchOnReceive(data); });
  }

  virtual void Send(S1::DataType& data) final {
    if (selected_stream2_) {
      stream2_.Send(data);
    } else {
      stream1_.Send(data);
    }
  }

  void SelectStream2() { selected_stream2_ = true; }

  void SelectStream1() { selected_stream2_ = false; }

 private:
  S1& stream1_;
  S2& stream2_;
  bool selected_stream2_{false};
};
}  // namespace robotics::network