#pragma once

#include <memory>

namespace robotics::network {
template <typename StreamT>
class StreamMuxer : public StreamT {
 public:
  StreamMuxer(std::shared_ptr<StreamT> stream1,
              std::shared_ptr<StreamT> stream2)
      : stream1_(stream1), stream2_(stream2) {}

  void Send(StreamT::DataType& data) final {
    if (selected_stream2_) {
      stream2_->Send(data);
    } else {
      stream1_->Send(data);
    }
  }

  void SelectStream2() { selected_stream2_ = true; }

  void SelectStream1() { selected_stream2_ = false; }

 private:
  std::shared_ptr<StreamT> stream1_;
  std::shared_ptr<StreamT> stream2_;
  bool selected_stream2_{false};
};
}  // namespace robotics::network