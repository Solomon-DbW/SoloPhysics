---
title: "SoloPhysics — Development Handbook"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# SoloPhysics — Development Handbook

> [!abstract] Project at a glance
> **Project:** SoloPhysics, a C++20 node–beam soft-body prototype developed on SoloLinux (Arch-based Linux). **Status at 31 October 2026:** working 3D GLAD 2/OpenGL 3.3 renderer, orbit camera, mouse node manipulation, spring–damper simulation, plastic deformation and beam fracture.
>
> **Audience:** a Python programmer moving into C/C++ and graphics programming. Each topic explains *what the line means*, *why it exists*, and *the closest Python analogy*.

## Start reading here

1. [[01 - Project Goals and Chronology]] — the development story, dates, decisions, and verified outputs.
2. [[02 - Environment and Build System]] — CMake, Ninja, Arch packages, files and compilation.
3. [[03 - C++ for a Python Developer]] — references, pointers, `const`, namespaces, classes, lambdas, templates and lifetime.
4. [[04 - Data Model and Source Architecture]] — `Node`, `Beam`, `PhysicsWorld`, `Renderer`.
5. [[05 - Forces and Spring Dynamics]] — Newton's laws, gravity, stiffness and damping.
6. [[06 - Numerical Integration and Timing]] — Euler, stability, 240 Hz accumulator.
7. [[07 - The Hanging Mass Experiment]] — theoretical equilibrium and measured verification.
8. [[08 - The Square Drop and Ground Contact]] — square beams, impact, and micro-bounce bug.
9. [[09 - Plasticity, Fracture and Damage]] — permanent deformation and failure.
10. [[10 - The First 2D Renderer]] — GLFW and legacy OpenGL as a teaching step.
11. [[11 - Interactive Mouse Controls]] — hover, dragging, callbacks and spring forces.
12. [[12 - 3D Cube and Orbit Camera]] — cube topology, matrices, picking and ray planes.
13. [[13 - Modern OpenGL and GLSL]] — shaders, VAO/VBO, GPU geometry and rendering.
14. [[14 - GLEW to GLAD Migration]] — the real Wayland initialization failure and its remedy.
15. [[15 - CMake Deep Dive]] — line-by-line current build file.
16. [[16 - Errors, Diagnostics and Lessons]] — real bugs and fixes in chronological order.
17. [[17 - Verification Tests and Exercises]] — reproducible tests and learning problems.
18. [[18 - Limits and Future Roadmap]] — what the engine cannot yet do.
19. [[19 - Bibliography and Attribution]] — websites, official documentation and evidence levels.
20. [[20 - C++ Quick Reference]] — high-frequency syntax in one place.

## How to study in Obsidian

Open this folder as an Obsidian vault. Use **Reading view** for rendered LaTeX, **Source mode** to copy C++, and the **Backlinks** panel to navigate. `[[Wiki links]]` connect related notes; `$$...$$` is a display equation; `` `code` `` is inline code; triple-backtick blocks hold C++, GLSL, shell, CMake and Mermaid diagrams.

> [!important] Evidence boundary
> The **current v0.4 files** in `Source Snapshots/v0.4-GLAD` were taken from the upgrade bundle available in the conversation, not by reading the live files on your computer. Earlier physics-core declarations and numerical outputs were documented during development, but the latest local `PhysicsWorld.cpp` was not supplied. Where code is reconstructed or illustrative, the notes explicitly say so. Do not interpret estimates as validated material parameters.

## Running the present version

```bash
cd ~/Projects/SoloPhysics
cmake -S . -B build-glad -G Ninja
cmake --build build-glad
./build-glad/solo_physics
```

The current known v0.4 scene has **8 nodes and 28 beams**, and uses `fixedDt = 1.0 / 240.0` seconds for stepping. The user confirmed that the GLAD version **works** on 31 October 2026.

## Formula snapshot

$$
\begin{aligned}
\mathbf F_g &= m\mathbf g \\
\mathbf F_s &= k(L-L_0)\hat{\mathbf d} \\
\mathbf F_d &= c\big[(\mathbf v_B-\mathbf v_A)\cdot\hat{\mathbf d}\big]\hat{\mathbf d} \\
\mathbf v_{n+1} &= \mathbf v_n + \Delta t\,\frac{\mathbf F_n}{m} \\
\mathbf x_{n+1} &= \mathbf x_n + \Delta t\,\mathbf v_{n+1}.
\end{aligned}
$$

See [[05 - Forces and Spring Dynamics]] and [[06 - Numerical Integration and Timing]] for signs and assumptions.
