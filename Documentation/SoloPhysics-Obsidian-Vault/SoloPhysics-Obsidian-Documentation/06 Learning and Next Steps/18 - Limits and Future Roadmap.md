---
title: "18 — Current Limits and a Path Toward Vehicle Simulation"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 18 — Current Limits and a Path Toward Vehicle Simulation

## Present capabilities (confirmed by source/conversation)

- 3D point-mass nodes and axial spring–damper beams.
- External gravity; fixed anchor nodes.
- Semi-implicit Euler, fixed-step application loop.
- Simplified flat-ground node contact and restitution.
- Yield-triggered rest-length change, broken-beam flags.
- OpenGL 3.3 line/point rendering, orbit/pan/zoom, screen-space picking and mouse spring dragging.
- Native GLFW context with GLAD 2; confirmed launched successfully after migration.

## Not yet established

A working 3D cube display is **not** the same as validated soft-body vehicle dynamics. Current limitations include: simplistic contact, no self-collision, no continuous collision detection, no proper constraints solver, no beam bending/buckling/torsion, no traction or suspension/wheels, no vehicle drivetrain, no contact patches, no material calibration, no deformable surface meshes, no broad-phase spatial partitioning, no replay-determinism guarantee, and no numerical performance benchmarks for thousands of beams.

## Suggested engineering order

| Phase | Priority | Why |
|---|---|---|
| 1 | Unit/regression testing | Guard physics correctness while changing architecture |
| 2 | Separate simulation library, scenario tests, UI | Easier to maintain/test than one large `main.cpp` |
| 3 | More robust ground contact and friction | Avoid velocity clamping artefacts |
| 4 | Better strain/damage history | Separate elastic/plastic/total strain; test energy and break criteria |
| 5 | Collision geometry and broad phase | Objects can actually crash into each other |
| 6 | Vehicle chassis + basic suspension | First vehicle-like dynamics |
| 7 | Surface mesh attached to node cage | Visible deformation beyond line segments |
| 8 | Constraint solvers, possibly XPBD | Address stiffness/time-step coupling and robust contacts |
| 9 | Profiling and scalability | Support realistic node/beam counts |

An eventual switch to **XPBD** or another constraint approach is a research direction, not an assertion that the current spring formulation is wrong. XPBD specifically targets problems of stiffness depending on timestep/solver iteration count; see Macklin et al., [XPBD paper](https://mmacklin.com/xpbd.pdf).

## Research questions to answer experimentally

1. How large can `stiffness` become at 240 Hz before noticeable instability?
2. How does the restitution threshold change the energy budget?
3. Is fracture triggered by instantaneous strain, accumulated history, stress, or energy release?
4. Does the rendering pick the nearest *visible* node, or just nearest in screen-space among all nodes?
5. How many nodes and beams can be simulated at 240 Hz on the target hardware?
6. How will a vehicle's tyres/suspension interact with the same contact system?

## Source discipline

Official BeamNG descriptions are a **conceptual inspiration** for node–beam simulation, but the SoloPhysics code remains its own simplified educational implementation. Do not infer BeamNG internals from matching terms or copy parameter units without verification.
