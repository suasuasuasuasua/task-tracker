{
  argparse,
  cmake,
  ftxui,
  gtest,
  lib,
  llvmPackages_21,
  nlohmann_json,
  spdlog,
  version,
}:
let
  inherit (llvmPackages_21) stdenv clang-tools;
  fs = lib.fileset;
  sourceFiles = fs.unions [
    ./Makefile
    ./VERSION
    ./package.nix
    (fs.fileFilter (file: file.hasExt "cpp") ./.)
    (fs.fileFilter (file: file.hasExt "h") ./.)
    (fs.fileFilter (file: lib.hasPrefix "CMakeLists" file.name) ./.)
  ];

  buildInputs = [
    argparse
    clang-tools
    cmake
    ftxui
    gtest
    nlohmann_json
    spdlog
  ];
in
stdenv.mkDerivation {
  inherit buildInputs version;

  pname = "task-tracker";
  src = lib.cleanSource (
    fs.toSource {
      root = ./.;
      fileset = sourceFiles;
    }
  );
  preConfigure =
    # bash
    ''
      export has_nix=true
    '';
  doCheck = true;
  installPhase =
    # bash
    ''
      mkdir -p $out/bin
      cp bin/* $out/bin
    '';
}
