{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/25.11";

  inputs.roboenv.url = "github:nnctroboticsclub/roboenv-nix";
  inputs.roboenv.inputs.nixpkgs.follows = "nixpkgs";

  inputs.nano.url = "github:nnctroboticsclub/nano";
  inputs.nano.inputs.nixpkgs.follows = "nixpkgs";
  inputs.nano.inputs.roboenv.follows = "roboenv";

  inputs.legacy-pkgs.url = "git+ssh://git@github.com/nnctroboticsclub/robopkgs-nix?dir=legacy";
  inputs.legacy-pkgs.inputs.nixpkgs.follows = "nixpkgs";
  inputs.legacy-pkgs.inputs.roboenv.follows = "roboenv";
  inputs.legacy-pkgs.inputs.nano.follows = "nano";

  outputs =
    {
      nixpkgs,
      legacy-pkgs,
      roboenv,
      nano,
      ...
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
      };
      roboPkgs = roboenv.legacyPackages.${system};
      legacyPkgs = legacy-pkgs.legacyPackages.${system};

      devPkgs = [
        roboPkgs.cmake-libs
        roboPkgs.clang-toolchain
        roboPkgs.cmsis5
        legacyPkgs.ikarashiCAN_mk2
        legacyPkgs.ikakoMDC
        legacyPkgs.IkakoRobomas
        legacyPkgs.MotorController
        nano.packages.${system}.default
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
