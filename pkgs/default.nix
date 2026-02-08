{
  argparse,
  cmake,
  ftxui,
  gtest,
  llvmPackages_21,
  nlohmann_json,
  pname,
  self,
  spdlog,
  version,
}:
let
  inherit (llvmPackages_21) stdenv clang-tools;
in
stdenv.mkDerivation {
  inherit pname version;

  src = builtins.path {
    path = self;
    name = pname;
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
