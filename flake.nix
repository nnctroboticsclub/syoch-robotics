{
  # 開発環境
  inputs.roboenv.url = "git+ssh://git@github.com/nnctroboticsclub/roboenv-nix";

  # roboenv 移行前のライブラリ群
  inputs.robopkgs.url = "git+ssh://git@github.com/nnctroboticsclub/robopkgs-nix";

  # Nano ライブラリ
  inputs.nano.url = "git+ssh://git@github.com/nnctroboticsclub/Nano";

  # f3-baremetal ライブラリ
  inputs.f3-baremetal.url = "git+ssh://git@github.com/nnctroboticsclub/f3-baremetal";

  outputs =
    {
      robopkgs,
      roboenv,
      nano,
      f3-baremetal,
      ...
    }:
    let
      system = "x86_64-linux";
      # roboenv の提供するパッケージ群
      roboPkgs = roboenv.legacyPackages.${system};
      # robopkgs の提供するパッケージ群
      roboLibs = robopkgs.legacyPackages.${system};
    in
    {
      # メインの開発環境 (`default` が識別子)
      devShells.x86_64-linux.default = roboPkgs.roboenv {
        # 開発環境の名前
        name = "Robotics Project";

        # 以下よく使う機能の定義
        # パラメータの詳細: https://github.com/nnctroboticsclub/roboenv-nix/tree/main/pkgs/roboenv

        rust.enable = true;
        STM32.enable = true;
        c_cpp.enable = true;
        c_cpp.toolchain = "clang";
        tool.usb.enable = true;

        frameworks = [
          {
            type = "StaticMbedOS";
            mbedTarget = "NUCLEO_F446RE";
          }
          {
            type = "STM32HAL";
            family = "f4";
          }
          {
            type = "STM32HAL";
            family = "f3";
          }
        ];

        cmakeInputs = [
          roboPkgs.cmake-libs
          roboLibs.club-legacy-libs
          roboLibs.srobo_base
          roboLibs.im920_rs
          roboLibs.ikarashiCAN_mk2
          roboLibs.ikakoMDC
          roboLibs.ikako_rohm_md
          roboLibs.MotorController
          roboLibs.IkakoRobomas
          roboLibs.can_servo
          roboLibs.Futaba_Puropo
          roboLibs.PS4_RX
          nano.packages.x86_64-linux.default
          f3-baremetal.packages.x86_64-linux.default
        ];
      };
    };
}
