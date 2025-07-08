#include <cstddef>

#include "managed_list.hpp"

namespace robotics::utils {
/// @tparam N max count of key-value pairs
template <typename K, typename V, std::size_t N>
class FixedMap {
 public:
  struct Pair {
    bool is_valid = false;

    K key;
    V* value = nullptr;
  };

 private:
  ManagedList<V, N> value_pool_;
  ManagedList<Pair, N> keys_;

 public:
  V* Find(const K& key) {
    for (auto&& key_data : keys_) {
      if (key_data.is_valid && key_data.key == key) {
        return key_data.value;
      }
    }

    return nullptr;
  }

  V& operator[](const K& key) {
    if (auto found = Find(key); found) {
      return *found;
    }

    auto new_key = keys_.New();
    if (!new_key) {
      return *value_pool_.New();
    }

    new_key->key = key;
    new_key->is_valid = true;
    new_key->value = value_pool_.New();

    return *new_key->value;
  }

  bool Contains(const K& key) { return Find(key) != nullptr; }

  auto begin() { return keys_.begin(); }
  auto end() { return keys_.end(); }
};
}  // namespace robotics::utils