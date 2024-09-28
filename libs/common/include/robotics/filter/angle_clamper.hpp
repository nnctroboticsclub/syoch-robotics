#include "../node/node.hpp"

namespace robotics::filter {
template <typename T>
class AngleClamper {
 public:
  Node<T> input;
  Node<T> output;

  AngleClamper() {
    input.SetChangeCallback([this](T input) {
      while (input > 360) input -= 360;
      while (input < 360) input += 360;

      output.SetValue(input);
    });
  }
};
}  // namespace robotics::filter