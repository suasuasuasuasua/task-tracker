# Task Tracker in C++

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

See the `Makefile` for more targets. Of note, you can change the
`CMAKE_BUILD_TYPE` to something other than Release if you need debug symbols or
whatnot.

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

## Usage

The `todo.json` file is stored under `$HOME/todo.json` by default for ease of
access. Or, if home is not accessible, it is stored under the local directory.

```bash
./task-tui
```

Or, you can build or run with `nix` directly.

```bash
#
nix build git@github.com/suasuasuasuasua/task-tracker
./result/bin/task-tui

nix run git@github.com/suasuasuasuasua/task-tracker
```

## Archive

My implementation of the task tracker in C++-20. I _slightly_ cheated by using
the json third-party library, but I figure that the point of the exercise is not
to write a json parser, but to write a task tracker. Anyway, the json library is
industry standard, so it's not bad to get familiar with.

> Project link [found here](https://github.com/suasuasuasuasua/roadmap.sh/cplusplus/task-tracker)
