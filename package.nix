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

  buildInputsVersions =
    let
      inherit (builtins)
        lessThan
        map
        sort
        concatStringsSep
        filter
        isAttrs
        hasAttr
        ;
      inherit (lib.lists) unique;
      validPackages = filter (p: isAttrs p && hasAttr "name" p) buildInputs;
      packages = map (p: "${p.name}") validPackages;
      sortedUnique = sort lessThan (unique packages);
      formatted = concatStringsSep "\n" sortedUnique;
    in
    formatted;
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
      # install binaries
      mkdir -p $out/bin
      cp bin/* $out/bin

      # list deps
      echo "${buildInputsVersions}" > $out/bin/deps.txt
    '';
}
