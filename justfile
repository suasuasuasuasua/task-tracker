PROJECT_NAME := "task-cli"
BUILD_DIR := "build"

default:
    @just --list

setup-release:
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -S . -B {{ BUILD_DIR }}

setup-debug:
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -S . -B {{ BUILD_DIR }}

build:
    cmake --build {{ BUILD_DIR }} --parallel

clean:
    cmake --build {{ BUILD_DIR }} --target clean

test:
    cmake --build {{ BUILD_DIR }} --target test

run:
    ./build/src/{{ PROJECT_NAME }}
