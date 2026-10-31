---
title: "14 — The GLEW Failure and GLAD 2 Migration"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 14 — The GLEW Failure and GLAD 2 Migration

## What actually happened on 31 October 2026

The user installed/upgraded packages using `pacman`. The first 3D renderer still used GLEW. A build of the existing sources succeeded, but running failed with:

```text
SoloPhysics error: Failed to initialise GLEW: Missing GL version
```

A key debugging detail: a GLAD ZIP was referenced in instructions but **not actually present in `~/Downloads`**. The first `unzip` command failed, so running CMake in a folder named `build-glad` still built the GLEW code (`-- Found GLEW ...`). This was a *patch-not-applied problem*, independent of the underlying graphics loader error.

After the ZIP was downloaded/extracted and the project rebuilt, the user confirmed: **"it works now"**.

## GLEW vs GLAD

Both are OpenGL function loaders. Modern OpenGL entry points are commonly retrieved at runtime from the active context, rather than being assumed statically exported by a system header/library. GLFW creates the window/context and provides `glfwGetProcAddress`. GLAD generates bindings for the requested API version. In this project, GLAD was selected to avoid the earlier GLEW initialization problem in the user's GLFW/Wayland setup.

> [!important] Root-cause precision
> The error alone does **not** prove Wayland/GLX was the sole technical cause. There was a suspected mismatch between GLEW's initialization path and the native context on that machine. The concrete verified facts are that GLEW failed, the old code persisted while the patch was absent, and the GLAD build worked after the patch was applied.

## GLAD 2, not GLAD 1

The v0.4 source uses:

```cpp
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// Only after glfwMakeContextCurrent(window):
const int loadedVersion = gladLoadGL(glfwGetProcAddress);
if (loadedVersion == 0 || !GLAD_GL_VERSION_3_3)
    throw std::runtime_error("Failed to load OpenGL 3.3 functions with GLAD");
```

`<glad/gl.h>` and `gladLoadGL(...)` are the GLAD 2 API used here; don't substitute `<glad/glad.h>` and `gladLoadGLLoader(...)` examples from GLAD 1.

**Correct order**: initialize GLFW → choose 3.3 core hints → create window → make context current → load GLAD → compile shader → create VAO/VBO → draw. The context must exist and be current when GLAD obtains entry points.

## Current CMake method

The file in the supplied ZIP pins an upstream GLAD Git commit and uses `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG 658f48e72aee3c6582e80b05ac0f8787a64fe6bb
    SOURCE_SUBDIR cmake
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_33 STATIC REPRODUCIBLE API gl:core=3.3 EXTENSIONS NONE)
```

This runs a generator (requires Python/Jinja2) and creates a CMake library target `glad_gl_core_33`. The pinned revision makes source selection repeatable; generated code and toolchains can still vary across environments. It downloads from GitHub on first configure unless cached.

## Debug checklist

```bash
# Did the source actually change?
grep -nE 'GLEW|glad|glfw' CMakeLists.txt
# Is the new include present?
grep -nE 'glew.h|glad/gl.h|gladLoadGL' src/rendering/Renderer.cpp
# Configure cleanly in a NEW directory
cmake -S . -B build-glad -G Ninja
cmake --build build-glad
./build-glad/solo_physics
```

This case is a good lesson: **a new build directory does not change the source code or dependencies by itself**. Confirm patch extraction before debugging runtime loader behaviour.

References: [GLAD upstream repository](https://github.com/Dav1dde/glad), [GLAD CMake API](https://github.com/Dav1dde/glad/blob/glad2/cmake/GladConfig.cmake), [GLFW guide](https://www.glfw.org/docs/latest/quick_guide.html), [CMake FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html).
