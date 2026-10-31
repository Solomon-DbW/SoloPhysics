# SoloPhysics v0.4 — OpenGL 3.3 3D renderer

This is a **drop-in upgrade pack**, not an independent full physics engine.

## What to replace

Copy these files into your existing `~/Projects/SoloPhysics` project, preserving paths:
- `CMakeLists.txt`
- `include/solophysics/Renderer.hpp`
- `src/rendering/Renderer.cpp`
- `src/main.cpp`

**Keep your existing** `include/solophysics/{Vector3D,Node,Beam,PhysicsWorld}.hpp` and `src/physics/PhysicsWorld.cpp`.

The project assumes your existing `Beam` includes `plasticallyDeformed` and `plasticDeformationEnabled`, and your `PhysicsWorld` includes `externalForceCallback`, because those were added in v0.3.

## Arch / SoloLinux dependencies

```bash
sudo pacman -S --needed glfw glew glm mesa cmake ninja
```

On older Arch package snapshots, GLFW may be packaged as `glfw-x11` rather than `glfw`.

## Build

```bash
cd ~/Projects/SoloPhysics
cmake -S . -B build -G Ninja
cmake --build build
./build/solo_physics
```

## Controls

- Left drag: grab node using screen-space picking and camera-facing drag plane
- Right drag: orbit camera
- Middle drag: pan
- Wheel: zoom
- F: toggle fixed anchor (node 0)
- G: toggle gravity
- Space: pause/resume
- R: reset
- Escape: quit

At launch, the cube is anchored and gravity is OFF for a deformation experiment. Press F and then G to drop the cube.

## Rendering

OpenGL 3.3 core profile, GLSL shaders, VAO/VBO, 3D perspective, depth testing, line-based grid, colour-coded beams, circular node points, and drag spring line. Broken beams are drawn grey for debugging.

## Caveats

The original physics implementation remains a simplified mass-spring model, and ground contacts remain per-node; it's not a BeamNG-equivalent solver. Fast, large pulling movements can stress the explicit integrator, although the mouse force is limited. Screenspace hovering may select an occluded node when their projections overlap. This prototype draws a line grid rather than a collidable mesh; collisions remain against `groundY`.
