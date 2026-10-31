---
title: "05 — Forces, Springs and Damping"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 05 — Forces, Springs and Damping

## 1. Newton's second law

For a node with constant mass $m$:

$$\sum \mathbf F = m\mathbf a, \qquad \mathbf a=\frac{\sum\mathbf F}{m}.$$

The world starts each timestep by clearing accumulated node forces, then adding external and internal forces:

```cpp
node.force = Vector3D{0.0f};
if (!node.fixed) node.force += node.mass * gravity;
```

If $m=1\,\mathrm{kg}$ and $g_y=-9.81\,\mathrm{m/s^2}$, the gravitational force is $F_y=-9.81\,\mathrm N$. A mass of 2 kg experiences $-19.62\,\mathrm N$, but *the same gravitational acceleration* when no other forces apply.

## 2. The direction of a beam

For nodes A and B:

$$
\begin{aligned}
\mathbf d &= \mathbf x_B-\mathbf x_A,\\
L &= \lVert\mathbf d\rVert,\\
\hat{\mathbf d} &= \frac{\mathbf d}{L},\\
e &= L-L_0.
\end{aligned}
$$

Here $L$ is current length, $L_0$ is the **current rest length**, and $e$ is extension in metres. A very small $L$ must be guarded before dividing by it (`if (length < 1e-6f) continue;`).

## 3. Hooke's law plus axial damping

The force on node **A** in the project's implementation is

$$
\boxed{\mathbf F_A=\Big[k(L-L_0)+c\big((\mathbf v_B-\mathbf v_A)\cdot\hat{\mathbf d}\big)\Big]\hat{\mathbf d}}.
$$

The force on B is $\mathbf F_B=-\mathbf F_A$ (Newton's third law). Positive extension pulls A toward B. If B moves away from A along the beam, the damping contribution opposes the relative motion by pulling A toward B and B toward A. Verify the sign *relative to the endpoint*; changing the sign of damping is a common instability bug.

Recorded logic:

```cpp
Vector3D delta = b.position - a.position;
float length = glm::length(delta);
if (length < 1e-6f) continue;
Vector3D direction = delta / length;
float extension = length - beam.restLength;
float relativeVelocity = glm::dot(b.velocity - a.velocity, direction);
float springForce = beam.stiffness * extension;
float dampingForce = beam.damping * relativeVelocity;
Vector3D force = (springForce + dampingForce) * direction;
if (!a.fixed) a.force += force;
if (!b.fixed) b.force -= force;
```

**Units:** $k$ N/m; $c$ N·s/m; $L-L_0$ m; $\dot L$ m/s; both resulting terms are N. These beams transmit **axial** load only, not moments or direct bending. Cross-bracing creates network-level resistance to shear.

## 4. One-dimensional worked calculation

Take $L_0=1.00$ m, $L=1.03$ m, $k=1000$ N/m, $c=10$ N·s/m, and $\dot L=0.2$ m/s:

$$F=k(0.03)+c(0.2)=30+2=32\,\mathrm N.$$

The magnitude is 32 N. Node A experiences $+32\hat{\mathbf d}$ and node B $-32\hat{\mathbf d}$. If the beam is compressing, extension and possibly axial velocity are negative, reversing the sign.

## 5. Spring energy and damping

An ideal undamped Hookean beam has potential energy:

$$U_s=\tfrac12 k(L-L_0)^2.$$

The axial damper dissipates energy at instantaneous rate

$$P_{d}=-c\dot L^2\le 0,$$

assuming the velocity projection and force signs above. This is a useful test for damping implementation. **Caveat:** node ground clamping, plastic changes to rest length and explicit integration alter the simple conservative-energy budget.

## 6. What is not modelled yet?

- No rotation or moment constraints at beam endpoints.
- No finite cross-section, distributed mass or bending stiffness for a single beam.
- No continuous collision detection or self-collision.
- No force-calibrated fatigue or crash material model.
- No physically derived plastic yield surface; current thresholds are intentionally simplified.

See [[09 - Plasticity, Fracture and Damage]] and [[18 - Limits and Future Roadmap]]. [BeamNG's node–beam explanation](https://www.beamng.com/game/about/physics/) is useful conceptual background but does not imply Solver equivalence.
