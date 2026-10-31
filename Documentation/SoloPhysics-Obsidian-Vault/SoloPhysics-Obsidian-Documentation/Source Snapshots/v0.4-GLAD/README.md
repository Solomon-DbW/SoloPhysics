# SoloPhysics v0.4 — GLAD 2 migration

This replaces **GLEW with GLAD 2** for the existing modern OpenGL 3.3 3D renderer.
It is an upgrade for an existing SoloPhysics v0.3/v0.4 physics project, not a complete standalone source tree.

## Installation (SoloLinux / Arch Linux)

```bash
sudo pacman -S --needed cmake ninja git python python-jinja glfw glm mesa
```

GLAD 2 is downloaded at **CMake configure time** using `FetchContent`, from an upstream
pinned commit (GLAD v2.0.6). Its loader source is **generated automatically** by CMake
using Python + Jinja2. You do not need `glew` or `glad` installed as system packages.
The first configure requires network access to GitHub. Subsequent builds reuse the
cached `build/_deps/glad-src` checkout.

The generated GLAD loader is for the **OpenGL 3.3 core profile** with no additional extensions.
It loads functions through `glfwGetProcAddress` for the current GLFW context.
This supports both **native Wayland (EGL)** and **X11/XWayland (GLX)** when GLFW and your
GPU driver provide compatible OpenGL 3.3 contexts. No forced X11 platform hint is used.

## Apply to your project

Back up or commit existing changes before extracting. The full upgrade ZIP contains the
updated `CMakeLists.txt` and `src/rendering/Renderer.cpp` along with the unchanged v0.4
`src/main.cpp` and `include/solophysics/Renderer.hpp`.

If you are already on v0.4, **only replace these 2 files**:

- `CMakeLists.txt`
- `src/rendering/Renderer.cpp`

Keep your existing physics engine files and any custom edits to `src/main.cpp` / `Renderer.hpp`.

## Configure and run

```bash
cd ~/Projects/SoloPhysics
cmake -S . -B build -G Ninja
cmake --build build
./build/solo_physics
```

The application will print e.g. `OpenGL loaded via GLAD 3.3` (or a newer compatible version).
If CMake was previously configured with GLEW, normal reconfigure is typically enough.
If you get stale dependency errors, try an **independent new build directory** instead:

```bash
cmake -S . -B build-glad -G Ninja
cmake --build build-glad
./build-glad/solo_physics
```

## Expected runtime

Retains all existing 3D functionality: orbit/pan/zoom camera; deformable 8-node cube,
28 beams, mouse force dragging, fixed-step simulation, ground, and structural damage.

- Left drag: grab node
- Right drag: orbit camera
- Middle drag: pan camera
- Scroll: zoom
- Space: pause/resume
- R: reset
- G: toggle gravity
- F: toggle anchor
- Escape: exit

### Troubleshooting

- **CMake cannot download glad:** confirm GitHub access and `git` installation.
- **`No module named jinja2`:** install `python-jinja` using pacman; confirm that the Python
  executable discovered by CMake can import Jinja2.
- **`Failed to create an OpenGL 3.3 core window`:** check `glxinfo -B` or `eglinfo`; GLAD
  cannot fix lack of OpenGL 3.3 driver support.
- **Window opens but nothing renders:** capture console output; check the shader program
  and look for GL errors.

### Technical notes

- GLAD 2 API is `<glad/gl.h>` plus `gladLoadGL(glfwGetProcAddress)`.
- GLAD 1 examples using `<glad/glad.h>` and `gladLoadGLLoader(...)` are **not** interchangeable.
- No external GLX loader is required by this project's OpenGL calls.
- The physics core is unchanged; existing simplified contact and spring damage assumptions still apply.
