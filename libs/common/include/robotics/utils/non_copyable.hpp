#pragma once

namespace robotics::utils {
/// @brief コピー不可を表すクラス
/// @details CRTP を用いる．
template <typename T>
class NonCopyable {
 protected:
  NonCopyable() = default;
  ~NonCopyable() = default;

  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};
}  // namespace robotics::utils