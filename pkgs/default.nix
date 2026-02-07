{
  argparse,
  clang-tools,
  cmake,
  ftxui,
  gtest,
  nlohmann_json,
  self,
  spdlog,
  stdenv,
  version,
}:
stdenv.mkDerivation {
  inherit version;

  pname = "task-cli";
  src = self;
  nativeBuildInputs = [
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
    mkdir -p $out/bin
    cp build/bin/* $out/bin
  '';
}
