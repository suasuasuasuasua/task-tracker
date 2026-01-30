{ self, pkgs, ... }:
pkgs.mkShellNoCC {
  inherit (self.checks.${pkgs.stdenv.hostPlatform.system}.git-hooks-check) shellHook;
  buildInputs = self.checks.${pkgs.stdenv.hostPlatform.system}.git-hooks-check.enabledPackages;

  packages = with pkgs; [
    cmake
    git
    llvmPackages_21.clang
    llvmPackages_21.clang-tools
    ninja
  ];
}
