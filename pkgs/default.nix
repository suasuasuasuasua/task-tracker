{
  self,
  cmake,
  ftxui,
  gtest,
  nlohmann_json,
  spdlog,
  stdenv,
  clang-tools,
}:
stdenv.mkDerivation {
  pname = "task-cli";
  version = "0.1.0";
  src = self;
  nativeBuildInputs = [
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
