{
  argparse,
  cmake,
  ftxui,
  gtest,
  lib,
  llvmPackages_21,
  nlohmann_json,
  pname,
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
    (fs.fileFilter (file: file.hasExt "cpp") ./app)
    (fs.fileFilter (file: file.hasExt "cpp") ./src)
    (fs.fileFilter (file: file.hasExt "cpp") ./tests)
    (fs.fileFilter (file: file.hasExt "h") ./inc)
    (fs.fileFilter (file: lib.hasPrefix "CMakeLists" file.name) ./.)
  ];
in
stdenv.mkDerivation {
  inherit pname version;

  src = lib.cleanSource (
    fs.toSource {
      root = ./.;
      fileset = sourceFiles;
    }
  );
  buildInputs = [
    argparse
    clang-tools
    cmake
    ftxui
    gtest
    nlohmann_json
    spdlog
  ];
  preConfigure =
    # bash
    ''
      export has_nix=true
    '';
  doCheck = true;
  installPhase = ''
    # install binaries
    mkdir -p $out/bin
    cp bin/${pname} $out/bin

    # list deps
    touch $out/bin/deps.txt
    cmake --version >> $out/bin/deps.txt
    clang --version >> $out/bin/deps.txt
  '';
}
