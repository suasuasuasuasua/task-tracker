{ self, pkgs, ... }:
let
  inherit (pkgs.stdenv.hostPlatform) system;
  inherit (pkgs.llvmPackages_21) stdenv;
in
pkgs.mkShell.override { inherit stdenv; } {
  inherit (self.checks.${system}.git-hooks-check) shellHook;
  buildInputs =
    self.packages.${system}.default.buildInputs
    ++ self.checks.${system}.git-hooks-check.enabledPackages;

  packages = builtins.attrValues {
    inherit (pkgs)
      git
      ;
  };
}
