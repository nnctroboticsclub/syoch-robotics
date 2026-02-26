# embedded_transform_target を使うため
include(TargetTransformer)

# NUCLEO_F446RE 向けの StaticMbedOS を扱うため
find_package(StaticMbedOS-NUCLEO_F446RE REQUIRED)

# 実行ファイルを作成する
add_executable(90-f4m-mybin src/main.cpp)

# 使用するライブラリの指定
target_link_libraries(90-f4m-mybin PUBLIC
  StaticMbedOS-NUCLEO_F446RE

  # lib/MyLib で作成したライブラリ
  MyLib
)

# StaticMbedOS の後処理を行う
static_mbed_os_nucleo_f446re_app_target(90-f4m-mybin)

# upload_... を実行できるようにする
embedded_transform_target(90-f4m-mybin)
