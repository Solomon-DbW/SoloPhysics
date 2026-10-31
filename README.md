<div align="center">

# SoloPhysics

### An experimental 3D soft-body physics engine built from scratch in C++20

**Node beam dynamics � Deformation and fracture � Interactive OpenGL visualisation**

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3%20Core-5586A4)
![GLAD](https://img.shields.io/badge/GLAD-2-2E8B57)
![Platform](https://img.shields.io/badge/Platform-Linux-informational?logo=linux&logoColor=white)
![Status](https://img.shields.io/badge/Status-Experimental-orange)

</div>

> [!IMPORTANT]
> **Work in progress.** SoloPhysics is a learning-driven physics and graphics project not a production-ready solver or an equivalent to BeamNG.drive. The **v0.4 upgrade pack** contains replacement application/rendering files and requires the physics core from the existing SoloPhysics project. If your GitHub repository contains the complete source tree, build it directly using the instructions below.

## Overview

SoloPhysics simulates structures as **point masses (nodes)** connected by **spring damper elements (beams)**. Each node responds to gravity, applied forces and collisions, while beams resist stretching and compression and can permanently deform or fracture when their strain exceeds configurable thresholds.

The current **v0.4** milestone adds a modern **OpenGL 3.3** renderer, an interactive 3D camera, mouse-based node manipulation, and a fully braced demonstration cube. The project is being developed as a foundation for exploring deformable vehicle structures, suspension and crash dynamics.

## Features

- **3D node beam simulation:** Nodes with mass, position, velocity and force; beams with rest length, stiffness and damping.
- **Fixed-step physics:** Semi-implicit Euler integration at **240 Hz**, independent of frame rendering.
- **Structural damage:** Configurable yield strain, plastic rest-length adjustment and beam fracture.
- **Ground interaction:** Simplified per-node ground contact, restitution and friction.
- **Modern graphics:** OpenGL 3.3 core profile, GLSL, VAO/VBO, perspective projection and depth testing.
- **GLAD 2 loader:** OpenGL functions are loaded through GLFW; no GLEW dependency.
- **Interactive tools:** Orbit/pan/zoom, screen-space node picking and mouse-spring dragging.
- **Live diagnostics:** The window title reports simulation state, gravity and damage counts.

### 3D demo structure

The default scene is a cube with **8 nodes and 28 beams**, arranged as:

| Connection type | Count |
| --- | ---: |
| Cube edges | 12 |
| Face diagonals | 12 |
| Body diagonals | 4 |
| **Total** | **28** |

The demo begins with **node 0 fixed** and **gravity disabled**, allowing you to stretch the cube before running a drop test.

## Getting started

### Requirements

- Linux with a working OpenGL **3.3 core-profile** driver
- A C++20 compiler, CMake **3.23+**, Ninja and Git
- GLFW, GLM and OpenGL drivers (Mesa on most Linux systems)
- Internet access for the **first CMake configure**, because CMake fetches GLAD 2

On Arch Linux / SoloLinux:

```bash
sudo pacman -S --needed base-devel git glfw glm mesa cmake ninja python python-jinja
```

> **GLAD is generated during CMake configuration.** Do not install GLEW for this build; it is no longer used. The project uses GLFW's OpenGL function resolver, so GLFW may use its available X11 or Wayland backend.

### Build and run

From the **complete SoloPhysics source tree**:

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/solo_physics
```

For a clean configuration when migrating from the former GLEW-based renderer:

```bash
cmake -S . -B build-glad -G Ninja
cmake --build build-glad
./build-glad/solo_physics
```

The executable is named `solo_physics`. A separate build directory avoids stale configuration from previous versions.

### Using the v0.4 upgrade pack

If you downloaded the upgrade pack **rather than a complete repository**, copy these files into your existing SoloPhysics project, preserving the relative paths:

```text
CMakeLists.txt
include/solophysics/Renderer.hpp
src/rendering/Renderer.cpp
src/main.cpp
```

Keep these existing engine files:

```text
include/solophysics/Vector3D.hpp
include/solophysics/Node.hpp
include/solophysics/Beam.hpp
include/solophysics/PhysicsWorld.hpp
src/physics/PhysicsWorld.cpp
```

The retained physics core must include `Beam::plasticallyDeformed`, `Beam::plasticDeformationEnabled` and `PhysicsWorld::externalForceCallback` (introduced during the v0.3 development stage). Then configure and build as above.

## Controls

| Input | Action |
| --- | --- |
| **Left mouse drag** | Pick and pull a node using a camera-facing drag plane |
| **Right mouse drag** | Orbit the 3D camera |
| **Middle mouse drag** | Pan the camera |
| **Mouse wheel** | Zoom in/out |
| **F** | Fix/unfix the anchor (node 0) |
| **G** | Toggle gravity |
| **Space** | Pause/resume |
| **R** | Reset the simulation |
| **Esc** | Close the application |

**Try it:** Drag a corner to deform the anchored cube. Press **F** to release the anchor, then **G** to switch gravity on and watch the cube fall onto the ground plane.

## How it works

### Forces and integration

A node accumulates forces from gravity, connected beams and interactive inputs. Its acceleration is given by Newton's second law:

$$
\mathbf{a} = \frac{\mathbf{F}_{\mathrm{total}}}{m}
$$

Each unbroken beam applies a spring damper force along the direction between two nodes:

$$
\mathbf{F}_{\mathrm{beam}} = \left[k(L-L_0)+c\left((\mathbf{v}_b-\mathbf{v}_a)\cdot\hat{\mathbf{d}}\right)\right]\hat{\mathbf{d}}
$$

Here, $L$ is the current beam length, $L_0$ its rest length, $k$ stiffness, $c$ damping, and $\hat{\mathbf{d}}$ the unit vector from node A to node B. Equal and opposite forces are applied to the endpoints where they are not fixed.

Semi-implicit Euler advances each movable node using a fixed timestep $\Delta t$:

$$
\mathbf{v}_{t+\Delta t}=\mathbf{v}_t+\mathbf{a}_t\Delta t, \qquad
\mathbf{x}_{t+\Delta t}=\mathbf{x}_t+\mathbf{v}_{t+\Delta t}\Delta t
$$

### Damage model

Beam strain is measured relative to its current rest length:

$$
\varepsilon=\frac{L-L_0}{L_0}
$$

A beam can change its rest length when its absolute strain exceeds `yieldStrain`; it fractures when its absolute strain reaches `breakStrain`. The demonstration uses a **simplified, non-calibrated** plasticity model, not a full constitutive material law. Broken beams stop transmitting forces and are displayed in grey for debugging.

### Rendering and interaction

The renderer converts world positions into the camera's perspective view using model/view/projection transforms. GLSL shaders draw the nodes, beams, a ground grid and a mouse-drag line. Node selection uses proximity in **screen pixels**; dragging projects the cursor onto a plane facing the camera and applies a capped spring damper force rather than teleporting the node.

## Project structure

```text
SoloPhysics/
    CMakeLists.txt
    include/
        solophysics/
            Vector3D.hpp
            Node.hpp
            Beam.hpp
            PhysicsWorld.hpp
            Renderer.hpp
    src/
        main.cpp
        physics/
            PhysicsWorld.cpp
        rendering/
            Renderer.cpp
```

The physics simulation is kept separate from rendering so that the engine can eventually support other front ends, test harnesses and simulation scenarios.

## Development milestones

| Version | Milestone |
| --- | --- |
| **v0.1** | First mass spring damper simulation and hanging-mass tests |
| **v0.2** | Four-node square drop, ground collisions and 2D OpenGL visualisation |
| **v0.3** | Mouse forces, camera controls, plastic deformation and beam fracture |
| **v0.4** | Braced 3D cube, orbit camera, shader-based renderer and **GLAD 2** migration |

## Current limitations

- Ground contact is resolved per node against `groundY`; the displayed grid is **not a collidable triangle mesh**.
- The integrator and damage model are intended for experimentation, not validated real-world crash predictions.
- Strong or rapidly varying forces can destabilise stiff spring systems; mouse force magnitude is capped, but the solver remains limited.
- Screen-space picking may select an occluded node where multiple projected nodes overlap.
- There is no vehicle model, tire/suspension system, self-collision solver or full rigid-body contact framework yet.

## Roadmap

- [ ] Separate material properties from per-beam damage state
- [ ] Improve contact stability and add 3D collision primitives
- [ ] Add solver validation tests and quantitative energy diagnostics
- [ ] Support larger meshes and configurable simulation scenes
- [ ] Develop a deformable chassis with suspension and wheel assemblies
- [ ] Add profiling, performance improvements and simulation recording

## Technology and references

- **C++20**   engine and application logic
- **[GLM](https://github.com/g-truc/glm)**   vectors and matrices
- **[GLFW](https://www.glfw.org/)**   windows, input and context creation
- **[OpenGL](https://www.khronos.org/opengl/)**   rendering API
- **[GLAD 2](https://github.com/Dav1dde/glad)**   generated OpenGL function loader
- **[CMake](https://cmake.org/)** and **[Ninja](https://ninja-build.org/)**   build tools

---

<div align="center">

**SoloPhysics**   learning physics simulation and graphics programming, one node and beam at a time.

</div>
