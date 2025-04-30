#pragma once

#include <concepts>
#include <cstdio>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>
#include "robotics/platform/panic.hpp"
#include "robotics/utils/non_copyable.hpp"
namespace robotics::network {
template <typename S1, typename S2>
requires(std::same_as<typename S1::DataType,
                      typename S2::DataType>) class StreamMuxer final
    : public S1::StreamType,
      public utils::NonCopyable<StreamMuxer<S1, S2>> {
  using S1_Storage = std::optional<S1>;
  using S2_Storage = std::optional<S2>;

 public:
  StreamMuxer(std::function<void(S1_Storage&, S2_Storage&)> initializer) {
    initializer(stream1_, stream2_);
    if (!stream1_.has_value() || !stream2_.has_value()) {
      printf("Stream initialization failed\n");
      system::panic("StreamMuxer initialization failed");
    }

    stream1_->OnReceive(
        [this](S1::DataType& data) { this->DispatchOnReceive(data); });

    stream2_->OnReceive(
        [this](S2::DataType& data) { this->DispatchOnReceive(data); });
  }

  virtual void Send(S1::DataType& data) final {
    if (selected_stream2_) {
      stream2_->Send(data);
    } else {
      stream1_->Send(data);
    }
  }

  void SelectStream2() { selected_stream2_ = true; }

  void SelectStream1() { selected_stream2_ = false; }

 private:
  std::optional<S1> stream1_;
  std::optional<S2> stream2_;
  bool selected_stream2_{false};
};
}  // namespace robotics::network