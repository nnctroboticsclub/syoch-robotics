#pragma once

namespace robotics::utils {
template <typename T>
class Singleton {
 public:
  static T& GetInstance() {
    static T instance;
    return instance;
  }

 protected:
  // Hidden 'Singleton' constructor
  Singleton() = default;
  ~Singleton() = default;

  // Hidden copy constructor
  Singleton(Singleton const&) = delete;
  Singleton& operator=(Singleton const&) = delete;
};
}  // namespace robotics::utils