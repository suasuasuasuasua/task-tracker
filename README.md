# Task Tracker in C++

A task tracker (todo list) implemented in C++ (targetting C++20) using FTXUI for
the tui library.

## Usage

The `todo.json` file is stored under `$HOME/todo.json` by default for ease of
access. Or, if home is not accessible, it is stored under the local directory.

```bash
./task-tui
```

Or, you can build or run with `nix` directly.

```bash
nix build git@github.com/suasuasuasuasua/task-tracker
./result/bin/task-tui

nix run git@github.com/suasuasuasuasua/task-tracker
```

## Build

Currently, task-tracker is building against these dependencies. See `shell.nix`
for more information.

```text
argparse==3.2
clang==21.1.7
cmake==4.1.2
ftxui==6.1.9
gtest==1.17.0
ninja==1.13.1
nlogmann_json==3.12.0
spdlog==1.15.3
```

You can build the program using `make`, which secretly calls the appropriate
`cmake` commands in the targets. See the `Makefile` for more details. Of note,
you can change the `CMAKE_BUILD_TYPE` to something other than 'Release' if you
need debug symbols and so on.

```bash
make build
./build/bin/task-tui
```

Or use `nix` to build.

```bash
nix build
./result/bin/task-tui
```

## Installing

The task-tracker can be installed using the `Makefile`. The install prefix can
be changed to a local directory in case you don't have `sudo` access.

```bash
# by default, installs to /usr/local/bin which requires sudo access
make install CMAKE_INSTALL_PREFIX=~/.local
```

## Development

I highly recommend using `nix` and `direnv` to automatically setup the
development shell and environment.

If not using `nix`, please see the 'build' section, as well as the
CMakeLists.txt files for the list of dependencies. CMake has been configured to
fetch the dependencies from GitHub using `FetchContent`.

> I have not tested out development environments without `nix` thoroughly.

## Archive

My implementation of the task tracker in C++-20. I _slightly_ cheated by using
the json third-party library, but I figure that the point of the exercise is not
to write a json parser, but to write a task tracker. Anyway, the json library is
industry standard, so it's not bad to get familiar with.

> Project link [found here](https://github.com/suasuasuasuasua/roadmap.sh/cplusplus/task-tracker)
