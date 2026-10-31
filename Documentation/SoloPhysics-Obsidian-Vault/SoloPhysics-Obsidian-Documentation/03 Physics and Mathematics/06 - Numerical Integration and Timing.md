---
title: "06 — Numerical Integration, Stability and Timesteps"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 06 — Numerical Integration, Stability and Timesteps

## 1. Continuous versus discrete motion

Newton's laws are differential equations:

$$\frac{d\mathbf x}{dt}=\mathbf v,\qquad \frac{d\mathbf v}{dt}=\mathbf a(\mathbf x,\mathbf v,t).$$

The CPU computes states at discrete times $t_n$. SoloPhysics uses **semi-implicit Euler**, also called symplectic Euler in this context:

$$\boxed{\begin{aligned}\mathbf v_{n+1}&=\mathbf v_n+\Delta t\,\mathbf a_n,\\\mathbf x_{n+1}&=\mathbf x_n+\Delta t\,\mathbf v_{n+1}.\end{aligned}}$$

Contrast explicit Euler: $\mathbf x_{n+1}=\mathbf x_n+\Delta t\,\mathbf v_n$. Using the *new* velocity for the position step generally behaves better for simple oscillators but does **not** make arbitrarily stiff springs stable.

```cpp
Vector3D acceleration = node.force / node.mass;
node.velocity += acceleration * dt;
node.position += node.velocity * dt;
```

A `fixed` node skips these updates. Nonpositive/NaN mass requires validation before division.

## 2. Choosing a small step

At $\Delta t=1/240$ seconds, the simulation attempts 240 updates for every simulated second. The single hanging-mass test initially used $1/120$ seconds; the later interactive application used $1/240$. A window rendering at 60 frames per second normally receives about four physics steps per frame.

A rough scale for a 1 kg body on a 1000 N/m spring is

$$\omega_n=\sqrt{k/m}=\sqrt{1000}\approx31.62\,\mathrm{rad/s},$$

$$\omega_n\Delta t\approx0.132\quad(240\,\mathrm{Hz}).$$

Small $\omega_n\Delta t$ generally improves numerical behaviour. Coupled beams have multiple vibration modes, so one scalar spring estimate is **not** a proof of overall stability. Damping too high can also cause numerical trouble with explicit schemes.

## 3. The frame-time accumulator

Current v0.4 `main.cpp` includes:

```cpp
constexpr double fixedDt = 1.0 / 240.0;
double previousTime = glfwGetTime();
double accumulator = 0.0;

while (!renderer.shouldClose()) {
    double now = glfwGetTime();
    double frameTime = std::clamp(now - previousTime, 0.0, 0.1);
    previousTime = now;
    accumulator += frameTime;
    while (accumulator >= fixedDt) {
        world.step(static_cast<float>(fixedDt));
        accumulator -= fixedDt;
    }
    renderer.render(world, grabbedNode, hoveredNode, mouseTarget);
}
```

This excerpt omits actual input processing, pause branch, and diagnostics for clarity. The *actual* source retains them. `glfwGetTime()` returns elapsed time; `std::clamp` limits a single frame to at most 0.1 s of physics catch-up. That means a lengthy stall can cause simulated time to fall behind real elapsed time. The accumulator preserves unsimulated fractional time within the chosen frame-time cap.

**Important:** fixed-step does not guarantee fully deterministic results across all hardware, floating-point modes or different input-event timings. The app currently samples mouse input once per render loop; it does not record/replay a timestamped input stream.

## 4. Critical damping and oscillations

For an isolated 1D mass–spring–damper:

$$m\ddot x+c\dot x+kx=0,$$

$$\omega_n=\sqrt{\frac{k}{m}},\qquad c_{crit}=2\sqrt{km},\qquad\zeta=\frac{c}{2\sqrt{km}}.$$

- $\zeta<1$: underdamped, oscillates while decaying.
- $\zeta=1$: critically damped in the *ideal isolated system*.
- $\zeta>1$: overdamped.

For $m=1$ kg, $k=100$ N/m, $c_{crit}=20$ N·s/m. With $c=5$, $\zeta=0.25$; with $c=0.6$, $\zeta=0.03$. These idealised formulas are excellent for validating the hanging-mass test but don't transfer directly to every mode of a 28-beam network.

## 5. Numerical checklist

1. Validate finite and positive masses and finite positions/velocities.
2. Avoid normalising zero-length beams.
3. Monitor maximum speed/force/strain if nodes explode or vanish.
4. Apply forces **after** resetting accumulators.
5. Ensure each action uses a known fixed $\Delta t$.
6. Use controlled tests with known answers before a complex vehicle.

Additional reading: Glenn Fiedler, [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/). See [[07 - The Hanging Mass Experiment]].
