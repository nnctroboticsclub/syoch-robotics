{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/25.11";

  # 開発環境
  inputs.roboenv.url = "git+ssh://git@github.com/nnctroboticsclub/roboenv-nix";

  # ライブラリ群
  inputs.robopkgs.url = "git+ssh://git@github.com/nnctroboticsclub/robopkgs-nix";
  inputs.robopkgs.inputs.nixpkgs.follows = "nixpkgs";
  inputs.robopkgs.inputs.roboenv.follows = "roboenv";

  outputs =
    {
      robopkgs,
      roboenv,
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
      packages.x86_64-linux.default = roboPkgs.rlib.buildCMakeProject {
        pname = "syoch-robotics";
        version = "v1.0.0";
        src = ./.;

        cmakeBuildInputs = [
          roboPkgs.cmake-libs
          roboPkgs.clang-toolchain
          roboPkgs.cmsis5
          roboLibs.ikarashiCAN_mk2
          roboLibs.ikakoMDC
          roboLibs.IkakoRobomas
          roboLibs.MotorController
          roboLibs.nano
        ];
        nativeBuildInputs = [ ];
      };
      # メインの開発環境 (`default` が識別子)
      devShells.x86_64-linux.default = roboPkgs.roboenv {
        name = "syoch-robotics";

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
          roboLibs.nano
          roboLibs.f3-baremetal
        ];
      };
    };
}
