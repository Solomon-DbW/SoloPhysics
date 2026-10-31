---
title: "15 — Line-by-Line CMake and Linking Guide"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 15 — Line-by-Line CMake and Linking Guide

## The actual supplied `CMakeLists.txt`

The current reference file can be opened in `Source Snapshots/v0.4-GLAD/CMakeLists.txt`. Its key lines:

```cmake
cmake_minimum_required(VERSION 3.23)
project(SoloPhysics LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
include(FetchContent)
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG 658f48e72aee3c6582e80b05ac0f8787a64fe6bb
    SOURCE_SUBDIR cmake
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_33 STATIC REPRODUCIBLE API gl:core=3.3 EXTENSIONS NONE)
add_executable(solo_physics
    src/main.cpp
    src/physics/PhysicsWorld.cpp
    src/rendering/Renderer.cpp)
target_include_directories(solo_physics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(solo_physics PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(solo_physics PRIVATE glm::glm glfw glad_gl_core_33)
target_compile_options(solo_physics PRIVATE -Wall -Wextra -Wpedantic)
```

## Decode it one line at a time

| Command | What it does | Python analogy |
|---|---|---|
| `cmake_minimum_required(...)` | minimum CMake needed to understand this build | minimum tool version |
| `project(... LANGUAGES C CXX)` | enable C and C++ compilation; GLAD generates C source | packaging metadata + compilers |
| `CMAKE_CXX_STANDARD 20` | request C++20 | choosing syntax/runtime target |
| `CMAKE_CXX_EXTENSIONS OFF` | prefer standard C++ over GNU-specific C++ language mode | strict portability configuration |
| `find_package(glm CONFIG REQUIRED)` | locate installed GLM CMake package target | resolve external dependency |
| `find_package(glfw3 CONFIG REQUIRED)` | locate GLFW package and target | resolve external dependency |
| `include(FetchContent)` | load built-in CMake dependency helper | import build script functionality |
| `FetchContent_Declare` | describe upstream Git source and revision | pin package source URL + commit |
| `FetchContent_MakeAvailable` | fetch/add dependency to configure | obtain/install build dependency |
| `glad_add_library(...)` | generate GLAD bindings/library | generate compile-time bindings |
| `add_executable` | declare target and source files | define application build |
| `target_include_directories` | where `#include` looks for local headers | interpreter import search path analogy |
| `target_compile_definitions` | define preprocessor flag | build-time configuration flag |
| `target_link_libraries` | link the app to required libraries | native linking, not `import` |
| `target_compile_options` | enable useful warnings | lint/checker options |

## CMake language syntax is not C++

`set(X 20)` has parentheses but is a CMake *command*, not a C++ function. Values are usually **strings/arguments** in the CMake language; variable interpolation uses `${VARIABLE}`, not Python f-strings. `PRIVATE`/`PUBLIC`/`INTERFACE` determine whether usage requirements propagate to depending targets, not the C++ access modifiers inside a class.

## Compile versus link errors

A missing `#include` file is a **preprocessing/compilation** problem. A function declared but never compiled into any linked object can cause `undefined reference` at **link** time. Missing `glfw` or generated `glad_gl_core_33` linkage is also a linking/configuration issue. Failure after the binary launches and prints a GLFW/GLEW/GLAD error is a **runtime** issue.

## Rebuild best practices

```bash
# Normal incremental build
cmake --build build-glad

# Reconfigure when CMakeLists.txt changes
cmake -S . -B build-glad -G Ninja

# Prefer fresh independent directory to troubleshoot cache issues
cmake -S . -B build-test -G Ninja
cmake --build build-test
```

Do not indiscriminately delete a directory that might contain something you placed there manually. Generated build directories can generally be recreated; keep source code outside them.

See [official CMake FetchContent documentation](https://cmake.org/cmake/help/latest/module/FetchContent.html), [GLAD CMake definition](https://github.com/Dav1dde/glad/blob/glad2/cmake/GladConfig.cmake).
