#pragma once

namespace robotics::binary {
/// @brief 同時に 2 箇所以上で使用できないバッファを取得する
void* GetTemporaryBuffer();
}  // namespace robotics::binary