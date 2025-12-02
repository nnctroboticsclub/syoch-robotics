#include "../node/node.hpp"

namespace robotics::filter {
template <typename T>
class AngleClamper {
 public:
  Node<T> input;
  Node<T> output;

  AngleClamper() {
    input >> ([this](T input_) {
      while (input_ > 360)
        input_ -= 360;
      while (input_ < 360)
        input_ += 360;

      output.SetValue(input_);
    });
  }
};
}  // namespace robotics::filter