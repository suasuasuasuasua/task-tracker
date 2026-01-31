{ self, pkgs, ... }:
pkgs.mkShell.override { inherit (pkgs.llvmPackages_21) stdenv; } {
  inherit (self.checks.${pkgs.stdenv.hostPlatform.system}.git-hooks-check) shellHook;
  buildInputs = self.checks.${pkgs.stdenv.hostPlatform.system}.git-hooks-check.enabledPackages;

  packages = with pkgs; [
    cmake
    ftxui
    git
    gtest
    ninja
    nlohmann_json
    spdlog
  ];
}
