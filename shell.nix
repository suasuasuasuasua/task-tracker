{ self, pkgs, ... }:
let
  inherit (pkgs.stdenv.hostPlatform) system;
  package = self.packages.${system}.default;

  inherit (package) buildInputs stdenv;
in
pkgs.mkShell.override { inherit stdenv; } {
  inherit (self.checks.${system}.git-hooks-check) shellHook;
  buildInputs = buildInputs ++ self.checks.${system}.git-hooks-check.enabledPackages;

  packages = builtins.attrValues {
    inherit (pkgs)
      git
      ;
  };
}
