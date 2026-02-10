#include <array>
#include <cstddef>

namespace robotics::utils {
template <typename T, std::size_t N>
class ManagedList {
  std::array<T, N> pool_ = {};
  size_t last_index_ = 0;

 public:
  T* New() {
    if (last_index_ >= N) {
      return nullptr;
    }

    return &pool_[last_index_++];
  }

  T* begin() { return pool_.data(); }
  T* end() { return pool_.data() + N; }
};

}  // namespace robotics::utils