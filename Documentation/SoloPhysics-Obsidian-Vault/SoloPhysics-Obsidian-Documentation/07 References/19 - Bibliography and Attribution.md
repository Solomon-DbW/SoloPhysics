---
title: "19 — Websites, Sources and Attribution"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 19 — Websites, Sources and Attribution

## Provenance statement — important

This bibliography has **two distinct classes of evidence**:

- **Project/author evidence (P):** user-supplied code, build logs, outputs, and conversation milestones. Those sources establish what SoloPhysics *actually did*.
- **External learning/reference (R):** authoritative websites checked while preparing this documentation (31 Oct 2026) or explicitly recommended/discussed during development. These explain third-party APIs and algorithms but are **not proof** that every page was personally consulted at the moment the original code was authored. The earlier assistant did not preserve an exhaustive browser-history ledger; therefore **an exhaustive list of every website previously visited cannot honestly be certified**.

This distinction avoids retroactively claiming websites were used when the evidence only shows they are relevant/recommended.

## Direct project evidence

| ID | Evidence | Use |
|---|---|---|
| P1 | Conversations and recorded milestone notes, 28–31 Oct 2026 | Design decisions, progression, suspended-mass and square-drop measurements |
| P2 | User's pasted `main.cpp` and `Renderer.cpp` excerpts from 2D iterations | Actual C++ syntax, callbacks and earlier debugging |
| P3 | CLI outputs for hanging mass, square drop, and improved contact | Numerical results ($1.9019$ m and resting square) |
| P4 | User's build logs, 31 Oct 2026: missing patch ZIP, GLEW failure | Loader transition diagnosis |
| P5 | `SoloPhysics-v0.4-GLAD-upgrade.zip` bundle available in the conversation | Verified v0.4 `CMakeLists.txt`, `main.cpp`, `Renderer.hpp`, `Renderer.cpp` |
| P6 | User statement, 31 Oct 2026, "it works now" | GLAD renderer runs on user's machine |

**Source completeness limit:** the current live repository and its newest local `PhysicsWorld.cpp` were not read in their entirety for this handbook. Source snapshots are the supplied GLAD upgrade bundle, not a Git clone of the user's project.

## Official documentation and educational sources

| ID | Website / precise URL | Why it matters | Status |
|---|---|---|---|
| R1 | [BeamNG: Soft-body Physics](https://www.beamng.com/game/about/physics/) | Nodes, massless beams, axial constraints, deformation/breaking | verified reference; conceptual inspiration |
| R2 | [BeamNG: Introduction to JBeam](https://docs.beamng.com/modding/vehicle/intro_jbeam/) | Structure, node/beam modelling, orientation | verified reference |
| R3 | [GLFW Getting Started](https://www.glfw.org/docs/latest/quick_guide.html) | Window/context creation, rendering loop, GLAD loader ordering | verified reference |
| R4 | [GLFW Input Guide](https://www.glfw.org/docs/latest/input.html) | Mouse/key polling, callbacks and window user pointers | verified reference |
| R5 | [GLAD upstream repository](https://github.com/Dav1dde/glad) | Generator and GLAD 2 function loading | verified reference; exact repository pinned by project |
| R6 | [GLAD CMake library definition](https://github.com/Dav1dde/glad/blob/glad2/cmake/GladConfig.cmake) | `glad_add_library(... API gl:core=3.3)` | verified reference |
| R7 | [CMake FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) | Fetch/build upstream dependency | verified reference |
| R8 | [GLM Manual](https://github.com/g-truc/glm/blob/master/manual.md) | `glm::vec3`, `glm::lookAt`, `glm::perspective`, `glm::project/unProject` | verified reference |
| R9 | [LearnOpenGL: Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle) | VBO, VAO, shaders and draw calls | verified educational reading |
| R10 | [LearnOpenGL: Camera](https://learnopengl.com/Getting-started/Camera) | View matrices and camera motion | verified educational reading |
| R11 | [Gaffer on Games: Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/) | Fixed-step accumulator rationale | verified educational reading |
| R12 | [cppreference: std::optional](https://en.cppreference.com/cpp/utility/optional) | Value-or-no-value state for selection/drag | verified C++ reference |
| R13 | [cppreference: std::function](https://en.cppreference.com/cpp/utility/functional/function) | Stored external force callback | verified C++ reference |
| R14 | [LearnCpp: Introduction to lambdas](https://www.learncpp.com/cpp-tutorial/introduction-to-lambdas-anonymous-functions/) | Captures, anonymous callable objects | verified learning reference |
| R15 | [Macklin, Müller and Chentanez: XPBD (2016)](https://mmacklin.com/xpbd.pdf) | Future constraint-solver research; not current algorithm | verified primary paper |

All external links were checked or retrieved during this documentation pass on **31 October 2026**. The exact content of external pages may change later. For authoritative specifications, prefer upstream docs; for tutorials, compare explanations against upstream APIs and compiler output.

## Licence/attribution awareness

SoloPhysics uses third-party libraries. GLM, GLFW, GLAD, OpenGL and CMake are distinct projects with their own documentation, licensing and APIs. This handbook paraphrases their concepts and contains small original instructional examples and user-project excerpts. It does **not** reproduce substantial portions of external tutorials or articles.

## Sources mentioned in original planning but not implemented

Early planning discussed position-based dynamics (PBD/XPBD) as a possible future strategy. It is not the solver currently in SoloPhysics. BeamNG references describe conceptual inspiration but the project has no claim of implementing the complete BeamNG algorithm.
