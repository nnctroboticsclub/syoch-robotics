#pragma once

namespace robotics::utils {
template <typename T>
class Singleton {
 public:
  static T& GetInstance() {
    static T instance;
    return instance;
  }

 private:
  // Hidden 'Singleton' constructor
  Singleton() = default;
  ~Singleton() = default;
};
}  // namespace robotics::utils