# Robotics

2024 交流ロボコンで作成したライブラリ詰め合わせセット

## 機能

- Node ベースのプログラミング
	- 入力
		- [x] `robotics::sensor::gyro::BNO055` BNO055 (yaw 角のみ)
	- 出力
		- [x] ikakoMDC との統合
			- `robotics::assembly::ikakoMDC` MDC 上の 1 モーターを扱うクラス
			- `robotics::node::ikakoMDCMotor` モーターの Node
 			- `robotics::node::ikakoMDCEncoder` エンコーダーの Node
		- [x] `robotics::node::BLDC` ブラシレスモーターを回せるクラス
		- [x] `robotics::node::DigitalOut` デジタル GPIO を `Node<bool>` として扱えるクラス
	- 複合
		- [x] ステア制御
			- `robotics::component::swerve::Motor` 1 ユニットを表すクラス
			- `robotics::component::swerve::Swerve` ステア全体を管理するクラス
		- [x] `robotics::filter::AngledMotor<T>` 角度指定でモーターを回すクラス
		- [x] `robotics::filter::IncAngledMotor<T>` `AngledMotor` の積分バージョン
		- [x] `robotics::utils::EMC` 複数の `Node<bool>` で複数の場所から `EMC` を切ることができるクラス
	- 変換
		- [x] `robotics::filter::AngleClamper<T>` 361 などを 360 に切り捨てたり -1 を 0 に上げたりするクラス
		- [x] `robotics::filter::AngleSmoother<T>` 359 -> 1 の変化を 359 -> 361 として処理できるフィルタクラス
		- [x] `robotics::filter::Joystick2Angle` 2 次元ベクトルを極座標変換するクラス
	- その他
		- [x] `robotics::filter::Muxer<T>` 複数の入力を切り替えるクラス
		- [x] `robotics::filter::PID<T>` PID
	-	[x] コントローラー
- マネージャー
	- `robotics::registry::ikakoMDC` ikakoMDC の MDC 自体を表すクラス
- 通信
	- `robotics::network::SimpleCAN` 基礎的な CAN 通信をするクラス
		- [x] メッセージ着のコールバック
		- [x] メッセージを送るときのコールバック
		- [x] 何もないときに実行されるコールバック
 	- `robotics::network::CANBase` `SimpleCAN` の基底クラス
	- `robotics::network::DistributedCAN` 応用的な CAN 通信をするクラス
		- [x] `SimpleCAN` で扱えるコールバック
		- [x] メッセージ ID 指定のコールバック
		- [x] Ping/Pong 通信
			- ESP32 などで定期的に Ping を送れば死活監視ができる
		- [ ] ストリーム通信
			- 任意のサイズのデータを送り会える
		- [x] KeepAlive の対応
			- このパケットを <300ms で送り続けることで接続が切れたことを検知できます
		- [x] ステータス表示
			- 現時点では `CANReady` `Ready` `InitializingGyro` `InitalizingBLDC0` `InitalizingBLDC1` の 5 状態のみサポートしてます
- プラットフォーム差吸収
	- Timer
		- [x] MBed
		- [ ] ESP-IDF
