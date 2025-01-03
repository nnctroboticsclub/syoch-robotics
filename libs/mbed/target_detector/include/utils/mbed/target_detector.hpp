#pragma once

//* Enumerator of targets
#define NUCLEO_F446RE 0
#define NUCLEO_F303K8 1

#define UtilsMbed_TargetIs(target) ((TARGET_NAME) == (target))
#define UtilsMbed_TargetSelect(target, true_value, false_value) \
  ((UtilsMbed_TargetIs(target)) ? (true_value) : (false_value))

namespace utils::mbed {
constexpr bool TargetIs(int target) {
  return target == TARGET_NAME;
}

template <typename T>
T Select(int target, T true_value, T false_value) {
  return TargetIs(target) ? true_value : false_value;
}
}  // namespace utils::mbed