{
  hooks = {
    check-merge-conflicts.enable = true;
    clang-format.enable = true;
    cmake-format.enable = true;
    deadnix.enable = true;
    end-of-file-fixer.enable = true;
    markdownlint.enable = true;
    nixfmt-rfc-style.enable = true;
    trim-trailing-whitespace.enable = true;
    update-latest-tag = {
      enable = true;
      # The name of the hook (appears on the report table):
      name = "update-latest-tag-version";
      # The command to execute (mandatory):
      entry = "./scripts/update-latest-tag.sh";
      # The pattern of files to run on (default: "" (all))
      # see also https://pre-commit.com/#hooks-files
      files = "";
      # List of file types to run on (default: [ "file" ] (all files))
      # see also https://pre-commit.com/#filtering-files-with-types
      # You probably only need to specify one of `files` or `types`:
      types = [ "text" ];
      # Exclude files that were matched by these patterns (default: [ ] (none)):
      excludes = [ ];
      # The language of the hook - tells pre-commit
      # how to install the hook (default: "system")
      # see also https://pre-commit.com/#supported-languages
      language = "system";
      # Set this to false to not pass the changed files
      # to the command (default: true):
      pass_filenames = false;
      # Which git hooks the command should run for (default: [ "pre-commit" ]):
      stages = [ "pre-commit" ];
    };
  };
}
