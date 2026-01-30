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
