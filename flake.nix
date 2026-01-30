{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    git-hooks.url = "github:cachix/git-hooks.nix";
    treefmt-nix.url = "github:numtide/treefmt-nix";
  };

  outputs =
    {
      self,
      nixpkgs,
      git-hooks,
      treefmt-nix,
      ...
    }:
    let
      supportedSystems = [
        "x86_64-linux"
        "x86_64-darwin"
        "aarch64-linux"
        "aarch64-darwin"
      ];
      forEachSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f pkgsFor.${system});
      pkgsFor = nixpkgs.lib.genAttrs supportedSystems (system: nixpkgs.legacyPackages.${system});

      # Eval the treefmt modules from ./treefmt.nix
      treefmtEval = forEachSystem (pkgs: treefmt-nix.lib.evalModule pkgs ./treefmt.nix);
    in
    {
      packages.x86_64-linux.default =
        let
          inherit (nixpkgs.legacyPackages.x86_64-linux)
            cmake
            ftxui
            gtest
            nlohmann_json
            ;
          inherit (nixpkgs.legacyPackages.x86_64-linux.llvmPackages_21)
            stdenv
            clang-tools
            ;
        in
        stdenv.mkDerivation {
          pname = "task-cli";
          version = "0.1.0";
          src = ./.;
          nativeBuildInputs = [
            clang-tools
            cmake
            ftxui
            gtest
            nlohmann_json
          ];
          configurePhase = ''
            export has_nix=true
          '';
          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-Dhas_nix=true"
          ];
          buildPhase = ''
            make build
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp build/task-cli build/task-tui $out/bin
          '';
        };

      #  for `nix fmt`
      formatter = forEachSystem (
        pkgs: treefmtEval.${pkgs.stdenv.hostPlatform.system}.config.build.wrapper
      );

      devShells = forEachSystem (pkgs: {
        default = import ./shell.nix {
          inherit pkgs self;
        };
      });

      checks = forEachSystem (pkgs: {
        formatting = treefmtEval.${pkgs.stdenv.hostPlatform.system}.config.build.check self;
        git-hooks-check = git-hooks.lib.${pkgs.stdenv.hostPlatform.system}.run {
          src = ./.;
          imports = [ ./git-hooks.nix ];
        };
      });
    };
}
