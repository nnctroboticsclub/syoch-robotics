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
      nixpkgs,
      robopkgs,
      roboenv,
      ...
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
      };
      roboPkgs = roboenv.legacyPackages.${system};
      roboLibs = robopkgs.legacyPackages.${system};

      devPkgs = [
        roboPkgs.cmake-libs
        roboPkgs.clang-toolchain
        roboPkgs.cmsis5
        roboLibs.ikarashiCAN_mk2
        roboLibs.ikakoMDC
        roboLibs.IkakoRobomas
        roboLibs.MotorController
        roboLibs.nano
      ];
    in
    {
      packages.x86_64-linux.default = roboPkgs.rlib.buildCMakeProject {
        pname = "syoch-robotics";
        version = "v1.0.0";
        src = ./.;

        cmakeBuildInputs = devPkgs;
      };
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

        cmakeInputs = devPkgs;
        buildInputs = [
          pkgs.commitizen
        ];
      };
    };
}
