#pragma once

// 名前衝突を避けるため、適当な名前空間にライブラリの提供する機能を配置する
// ここでは、 mylib (ライブラリ名) としている

namespace mylib {

/// @brief 2 値を加算する
int Add(int x, int y);

}  // namespace mylib