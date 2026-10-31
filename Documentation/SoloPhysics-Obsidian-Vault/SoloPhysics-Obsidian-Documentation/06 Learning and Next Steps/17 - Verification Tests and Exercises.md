---
title: "17 — Tests, Learning Exercises and Worked Answers"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 17 — Tests, Learning Exercises and Worked Answers

## A. Regression test matrix

| Test | Setup | Expected trend/criterion |
|---|---|---|
| Gravity-only drop | unconnected 1 kg node, no ground impact | $v_y\approx -9.81t$, $y\approx y_0-\frac12gt^2$; discretisation causes small error |
| Zero-force isolation | one node, gravity zero | position and velocity stay unchanged |
| Equal/opposite beam loads | 2 free nodes linked by one stretched spring | internal forces sum to approximately zero |
| Hanging spring equilibrium | anchor 3 m, rest 1 m, $m=1$, $k=100$ | settled height approximately **1.9019 m** |
| Damping comparison | compare $c=5$ versus $c=0.6$ | smaller damping oscillates longer |
| Square pre-impact | 4 nodes, equal masses, initial square | common downward acceleration before contact |
| Resting ground | square after impact and settling | bottom $y=0$, tiny $v_y$ after correction |
| Plasticity disabled | pull beam above `yieldStrain` | rest length stays unchanged, history flag false |
| Plasticity enabled | pull above yield without breaking | rest length changes; history flag becomes true |
| Fracture | pull past `breakStrain` | `broken=true`, no spring force in future steps |
| GLAD startup | OpenGL 3.3 driver/context | logs a loaded version; shader program links |
| 3D picking | orbit around cube and click a node | selected node index remains valid |

## B. Exercise 1 — Translate Python to C++

Python:

```python
mass = 1.0
position = [0.0, 2.0, 0.0]
velocity = [0.0, 0.0, 0.0]
```

Exercise: create a `Node` in C++ and set the same state. **Answer:**

```cpp
Node node;
node.mass = 1.0f;
node.position = Vector3D{0.0f, 2.0f, 0.0f};
node.velocity = Vector3D{0.0f};
```

## C. Exercise 2 — Calculate spring force

A 1 m rest beam is 1.02 m long, $k=650$ N/m, axial relative velocity 0.1 m/s, $c=12$ N·s/m. Find scalar force. **Answer:**

$$F=650(0.02)+12(0.1)=13+1.2=14.2\,\mathrm N.$$

This assumes `direction` points A→B and positive relative velocity means endpoints are separating.

## D. Exercise 3 — Interpret the lambda

```cpp
auto fn = [&](PhysicsWorld& world) {
    if (!grabbedNode) return;
    world.nodes[*grabbedNode].force += Vector3D{1, 0, 0};
};
```

Explain why the compiler allows access to `grabbedNode` even though it is not a parameter. **Answer:** `[&]` captures used enclosing local variables by reference; `fn` can read the latest `grabbedNode`, assuming its lifetime remains valid. `std::optional` presence is tested before dereferencing.

## E. Exercise 4 — Derive static equilibrium

Change the hanging-mass spring to $k=200$ N/m, $m=1$ kg, same anchor/rest length. Predict position. **Answer:** extension $mg/k=0.04905$ m, so

$$y_{eq}=3-(1+0.04905)=1.95095\,\mathrm m.$$

## F. Exercise 5 — Why 28 beams?

For eight distinct nodes, an edge between every unordered pair creates $8\times7/2=28$ beams. This is the complete graph $K_8$. Not all 28 are necessary for a rigid braced cube; the current topology is a simple heavily reinforced learning structure.

## G. Exercise 6 — Debug a missing force

You add `world.nodes[3].force += F;` just before calling `world.step(dt)`, but the node doesn't respond. Where does it go wrong? **Answer:** `resetForces()` zeros it inside the timestep; supply it via `externalForceCallback` (or a formal persistent external-force input system) before beam solution/integration.

## H. Exercise 7 — Change OpenGL safely

Explain why these lines must be ordered:

```cpp
glfwMakeContextCurrent(window);
int loadedVersion = gladLoadGL(glfwGetProcAddress);
```

**Answer:** OpenGL entry-point lookup depends on a current context. Trying to load before binding a context may fail, even if the compiler and linker are happy.

## I. Suggested exercise roadmap

1. Write a tiny C++ test executable that compiles only `PhysicsWorld.cpp` plus tests; no renderer.
2. Compare analytical and simulated oscillator periods over multiple timesteps.
3. Record total energy (kinetic + spring + gravitational) with no damping/ground and explain integration drift.
4. Visualise active-beam strain as a continuous gradient while keeping plastic history a distinct overlay.
5. Expose material parameters in a configuration file rather than hardcoding each scenario.
6. Add mouse-input replay so regression tests don't depend on how quickly you move the mouse.

Each task depends on concepts already encountered in the project; keep a lab note recording hypothesis, method, result and discrepancies.
