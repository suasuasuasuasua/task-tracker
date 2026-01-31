# Task Tracker in C++

My implementation of the task tracker in C++-20. I _slightly_ cheated by using
the json third-party library, but I figure that the point of the exercise is not
to write a json parser, but to write a task tracker. Anyway, the json library is
industry standard, so it's not bad to get familiar with.

> Project link [found here](https://github.com/suasuasuasuasua/roadmap.sh/cplusplus/task-tracker)

## Build

Currently, task-tracker is building against these dependencies. See `shell.nix`
for more information.

```text
clang-tidy==21.1.7
clang==21.1.7
cmake==4.1.2
make==4.4.1
ninja==1.13.1
```

See the `Makefile` for more recipes on building.

```bash
make build
./build/task-cli help
```

Or use `nix` to build.

```bash
nix build
./result/bin/task-cli help
```

## Usage

The `todo.json` file is stored under `$HOME/todo.json` by default for ease of
access. Or, if home is not accessible, it is stored under the local directory.

See all the commands.

```bash
./task-cli
./task-cli help
```

Add, modify, and delete

```bash
./task-cli add UID "SHORT DESC"
./task-cli modify UID "SHORT DESC"
./task-cli delete UID

./task-cli mark-in-progress UID
./task-cli mark-done UID
```

List all tasks.

```bash
./task-cli list
./task-cli list todo
./task-cli list in-progress
./task-cli list done
```

Run with `nix` directly. This method is slightly slower.

```bash
nix run \
    "git+ssh://git@github.com/suasuasuasuasua/roadmap.sh?ref=main&dir=cplusplus/task-tracker"
```

## Future Work

- Add option to save in different places

## Versioning and Releases

This project uses [Semantic Versioning](https://semver.org/) and follows [Conventional Commits](https://www.conventionalcommits.org/) for commit messages.

### For Maintainers

#### Bumping Version

To bump the version manually, use the `make bump` command:

```bash
# Bump patch version (e.g., 1.0.0 -> 1.0.1)
make bump VERSION=patch

# Bump minor version (e.g., 1.0.0 -> 1.1.0)
make bump VERSION=minor

# Bump major version (e.g., 1.0.0 -> 2.0.0)
make bump VERSION=major

# Set a specific version
make bump VERSION=1.2.3
```

This will update the version in:
- `VERSION` file
- `CMakeLists.txt`
- `flake.nix`
- `.cz.yaml`

#### Creating a Release

1. Bump the version (see above)
2. Update `CHANGELOG.md` with the changes
3. Commit the changes:
   ```bash
   git add -A
   git commit -m "chore: bump version to X.Y.Z"
   ```
4. Create and push a tag:
   ```bash
   git tag -a vX.Y.Z -m "Release vX.Y.Z"
   git push origin main
   git push origin vX.Y.Z
   ```
5. The GitHub Actions workflow will automatically:
   - Build binaries for all supported platforms
   - Create a GitHub Release
   - Attach downloadable artifacts (accessible to non-authenticated users)

### For Users

#### Installation

Download the latest release from the [Releases page](https://github.com/suasuasuasuasua/task-tracker/releases).

Or use Nix with a specific version:
```bash
# Run latest release
nix run github:suasuasuasuasua/task-tracker

# Run a specific version
nix run github:suasuasuasuasua/task-tracker/v0.1.0
```
