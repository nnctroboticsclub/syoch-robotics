#pragma once

#include <ikarashiCAN_mk2.h>
#include <array>

#include "ikakoMDC.hpp"

#include <robotics/network/dcan.hpp>

namespace robotics::registry {
class ikakoMDC {
  using MotorPair = assembly::ikakoMDCPair<float>;
  ::ikakoMDC motors_[4];
  ikakoMDC_sender sender_;
  std::array<MotorPair, 4> motor_nodes_;
  ikarashiCAN_mk2* linked_ican_;

  int report_counter = 0;

  /// @internal
  /// @brief 速度レポートを送信する
  void ReportSpeed(robotics::network::DistributedCAN& can, uint8_t id);

  /// @internal
  /// @brief エンコーダーレポートを送信する
  void ReportEncoder(robotics::network::DistributedCAN& can, uint8_t id);

 public:
  /// @param can 連携する ikarashiCAN_mk2 インスタンスへのポインタ
  /// @param mdc_id MDC の ikarashiCAN ID
  ikakoMDC(ikarashiCAN_mk2* can, int mdc_id);

  /// @brief 毎ループに呼ぶ関数
  /// @details エンコーダーの状態を取得して、 MotorPair の状態を更新する
  void Tick();

  /// @brief DCAN にレポートを送信する
  /// @details
  ///   副作用として report_counter がインクリメントされる
  ///   偶数回目は速度レポート、奇数回目はエンコーダーレポートを送信する
  /// @param can DCAN インスタンス
  /// @param id レポート ID (0-15)
  void ReportTo(robotics::network::DistributedCAN& can, uint8_t id);

  /// @brief 速度構造体を送信する
  /// @return 0: 失敗, 1: 成功
  int Send();

  /// @brief 指定したインデックスの MotorPair を取得する
  /// @param index インデックス (0-3)
  /// @return MotorPair の参照
  MotorPair& GetNode(int index);
};
}  // namespace robotics::registry
