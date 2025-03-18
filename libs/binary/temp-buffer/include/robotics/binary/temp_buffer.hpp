#pragma once

namespace robotics::binary {
/// @brief 同時に 2 箇所以上で使用できないバッファを取得する
void* GetTemporaryBuffer();

/// @brief アプリケーションサイドのバッファを取得する
void* GetTemporaryAppBuffer();
}  // namespace robotics::binary