cmake_minimum_required(VERSION 3.26)

# Disable in-source builds since they would override the Makefile
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed")
endif()

file(READ "VERSION" ver)
string(REPLACE "v" "" ver ${ver})
string(REPLACE "\n" "" ver ${ver})
project(
  TaskTracker
  LANGUAGES CXX
  VERSION ${ver})

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(DEFINED ENV{has_nix})
  # NOTE: nix requires extra steps because include folders are in the nix store
  # https://mzhang.io/posts/2022-03-03-clangd-in-nix/
  #
  # though, this is not good practice according to cmake docs
  if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  endif()
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

message(STATUS "${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for the project
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

# third-party deps
if(DEFINED ENV{has_nix})
  message("Using nix deps")
  find_package(GTest 1.17.0 REQUIRED)
  find_package(argparse 3.2 REQUIRED)
  find_package(ftxui 6.1.9 REQUIRED)
  find_package(nlohmann_json 3.12.0 REQUIRED)
  find_package(spdlog 1.15.3 REQUIRED)
else()
  message("Fetching deps from git")
  include(FetchContent)
  FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2)
  FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9 # Replace with a version, tag, or commit hash
  )
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
  )
  FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
  FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.15.3.tar.gz)

  FetchContent_MakeAvailable(argparse ftxui googletest json spdlog)
endif()

add_subdirectory(app)
add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(testing_enabled ON)
else()
  set(testing_enabled OFF)
endif()

option(PACKAGE_TESTS "Build the tests" ${testing_enabled})
if(PACKAGE_TESTS)
  add_subdirectory(tests)
  enable_testing()
endif()
