# makefiles

色々なタスクランナー/ビルドシステムのテンプレ/関数が入ってます。

## 追加されるルール

### sdl (Show device list)

追加されているデバイスリストを表示します
2024 交流ロボコン C チームでの出力はこんな感じです

```
--- Device List ---
stm32-s1-066EFF303435554157121019 stm32-s2-066AFF495057717867162927 esp32-e
```

## ファイル構成

- all.mk: すべてのmakefileをインクルードします。
- sess.tmux.mk: tmuxのセッションを管理する makefile です
- init.mk: 様々な初期化処理を行う makefile です
- device_list.mk: デバイスのリストを表示する makefile です
- utils
  - utils.usb.mk: lsusb 相当のやつが入ってます
  - utils.misc.mk: その他の関数が入っています
- net
  - net.NAS.mk: NAS に関する関数が入っています
  - net.vpn.mk: VPN に関する関数が入っています
- mcu
  - ESP32.mk: ESP32 でよく使うタスクが入っています
  - STM32.mk: STM32 でよく使うタスクが入っています
- env
  - NAS.mk: NAS に関する環境変数が入っています
  - net.mk: ネットワークに関する環境変数が入っています
  - vpn.mk: VPN に関する環境変数が入っています

## Makefile 例

(korobo-2024-c)[https://github.com/nnctroboticsclub/korobo-2024-c] より引用
```makefile
all:

-include env.home.mk
-include syoch-robotics/makefile.d/all.mk

S1_LOG_TAG      := "SerialProxy (UART: 1)"
S1_SKIP_COMPILE ?= 0

S2_LOG_TAG      := "SerialProxy (UART: 2)"
S2_SKIP_COMPILE ?= 0

ESP_SKIP_COMPILE ?= 0

$(eval $(call STM32_DefineRules,s1,$(ESP32_IP),$(S1_LOG_TAG),/workspaces/korobo2023/stm32-main,$(S1_SKIP_COMPILE),NUCLEO_F446RE,/mnt/st1,066EFF303435554157121019))
$(eval $(call STM32_DefineRules,s2,$(ESP32_IP),$(S2_LOG_TAG),/workspaces/korobo2023/stm32-enc,$(S2_SKIP_COMPILE),NUCLEO_F446RE,/mnt/st2,066AFF495057717867162927))
$(eval $(call ESP32_DefineRules,e,/workspaces/korobo2023/esp32,$(ESP_SKIP_COMPILE)))
```

### 解説

-include の行で env.home.mk と all.mk をインクルードしています。
ここで env.home.mk は一部の変数をオーバーライドするデータで、 all.mk はこのファイル群の Makefile をインクルードするファイルです。

`$(eval $(call...))` の行はルールを一括定義してます。これにより次のようなルールが定義されます。（一部）
- `ms1`: Monitor S1(=STM32 メインメインマイコン)
- `fs1`: Flash S1(=STM32 メインメインマイコン)
- `me`: Monitor e(=ESP32)
- `lu`: List Usb

## よく使う（であろう）関数

### STM32_DefineRules

引数
(タグ s1, s2 など) (PC からみた ESP32 の IP、 無線書き込みに使用) (ロギングタグ)
(プロジェクトディレクトリのルート) $(コンパイルをスキップするか)
(ボード種別) (ST-Link のマウント先) (ST-Link の USB シリアル番号)

### ESP32_DefineRules

引数
(タグ e など) (プロジェクトディレクトリのルート) $(コンパイルをスキップするか)