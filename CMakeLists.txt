cmake_minimum_required(VERSION 3.26)
project(TaskTracker LANGUAGES CXX)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_CXX_FLAGS -Wall)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# NOTE: nix requires extra steps because include folders are in the nix store
# https://mzhang.io/posts/2022-03-03-clangd-in-nix/
#
# though, this is not good practice according to cmake docs
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# Check the build type
if(NOT CMAKE_BUILD_TYPE)
  set(default_build_type "Debug")
  message(
    STATUS
      "Set the build type to `${default_build_type}` as none was specified.")
  set(CMAKE_BUILD_TYPE
      ${default_build_type}
      CACHE STRING "Chooce the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

message(STATUS "TaskTracker Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for TaskTracker
set(TaskTracker_Version_Major 0)
set(TaskTracker_Version_Minor 1)
set(TaskTracker_Version_Patch 0)
set(TaskTracker_Version_Status "-dev")
set(PROJECT_VERSION
    "${TaskTracker_Version_Major}.${TaskTracker_Version_Minor}.${TaskTracker_Version_Patch}${TaskTracker_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(FetchContent)

# third-party deps
if(DEFINED ENV{json_src})
  message("Fetching json source from env var: $ENV{json_src}")
  FetchContent_Declare(json SOURCE_DIR "$ENV{json_src}")
else()
  message("Fetching json source from git")
  FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
endif()
FetchContent_MakeAvailable(json)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
