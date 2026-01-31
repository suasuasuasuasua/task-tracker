cmake_minimum_required(VERSION 3.26)

# Disable in-source builds since they would override the Makefile
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed")
endif()

project(TaskTracker LANGUAGES CXX)

# TODO: enable on dev or ci builds? takes a while to get through ftxui
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
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
      CACHE STRING "Choose the build type." FORCE)
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
if(DEFINED ENV{has_nix})
  message("Using nix deps")
  find_package(GTest 1.17.0 REQUIRED)
  find_package(ftxui 6.1.9 REQUIRED)
  find_package(nlohmann_json 3.12.0 REQUIRED)
  find_package(spdlog 1.15.3 REQUIRED)
else()
  message("Fetching deps from git")
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

  FetchContent_MakeAvailable(ftxui googletest json spdlog)
endif()

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
