{
  argparse,
  cmake,
  ftxui,
  gtest,
  llvmPackages_21,
  nlohmann_json,
  self,
  spdlog,
  version,
}:
let
  inherit (llvmPackages_21) stdenv clang-tools;
in
stdenv.mkDerivation {
  inherit version;

  pname = "task-tracker";
  src = builtins.path {
    path = self;
    name = "task-tracker";
  };
  buildInputs = [
    argparse
    clang-tools
    cmake
    ftxui
    gtest
    nlohmann_json
    spdlog
  ];
  configurePhase = ''
    export has_nix=true
  '';
  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
  ];
  buildPhase = ''
    make build
  '';
  installPhase = ''
    # install binaries
    mkdir -p $out/bin
    cp build/bin/* $out/bin

    # list deps
    touch $out/bin/deps.txt
    cmake --version >> $out/bin/deps.txt
    clang --version >> $out/bin/deps.txt
  '';
}
