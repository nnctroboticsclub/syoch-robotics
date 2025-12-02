#pragma once

#include "node.hpp"

namespace robotics::node {
template <typename T>
class Motor : public Node<T> {
 public:
  Node<T> factor;

 private:
  /**
   * @brief Set the Speed
   * @param speed in the range [-1, 1]
   */
  virtual void SetSpeed(T speed) = 0;

  void Update() { SetSpeed(GetSpeed()); }

 public:
  Motor() {
    factor.SetValue(1);

    *this >> [this](T) {
      this->Update();
    };
    factor >> [this](T) {
      this->Update();
    };
  }

  T GetSpeed() {
    auto factor_value = factor.GetValue();
    auto speed = this->GetValue();
    auto effective_speed = factor_value * speed;

    return effective_speed;
  }
};
}  // namespace robotics::node