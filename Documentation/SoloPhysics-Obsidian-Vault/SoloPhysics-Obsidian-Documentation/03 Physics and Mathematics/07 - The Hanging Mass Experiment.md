---
title: "07 — First Verification: Suspended Mass"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 07 — First Verification: Suspended Mass

## Experiment design

The first successful validation used a **single 1 kg mass** suspended from a fixed anchor at $y=3$ m by a beam of initial length $L_0=1$ m, stiffness $k=100$ N/m and damping initially about $5$ N·s/m. Gravity pointed down with magnitude $9.81$ m/s².

```
Anchor at y=3.0000 m   [fixed]
            |
            | beam, L0=1 m
            |
Mass initially y=2.0000 m [1 kg]
            ↓ gravity
```

## Predict the resting height *before* simulation

At equilibrium, the spring must exert $mg=9.81$ N upward. The spring therefore extends by

$$\Delta L=\frac{mg}{k}=\frac{1\times9.81}{100}=0.0981\,\mathrm m.$$

Its settled length is $L=1.0981$ m. Because the mass is suspended below the anchor,

$$\boxed{y_{eq}=3.0-1.0981=1.9019\,\mathrm m}.$$

The actual CLI output converged to approximately **1.9019 m**, matching this prediction at the reported precision. This gave initial evidence that gravity, axial spring direction and integration were coherently implemented.

## Damping experiment

Keeping $m=1$, $k=100$ and changing damping from roughly $c=5$ to $c=0.6$ caused oscillations to persist longer, as expected from $\zeta=c/(2\sqrt{km})$. A key experimental precaution was to disable plastic deformation (`plasticDeformationEnabled=false`) and raise `breakStrain` so the spring's rest length would not change during this elastic baseline test.

> [!tip] Why this is a strong engineering test
> A test is most useful when you can calculate an expected result without relying on the program. Otherwise, incorrect code may still draw a convincing animation.

## Lessons recorded

- Be sure to save `main.cpp` before rebuilding; an earlier test accidentally ran with unchanged source and misleadingly similar output.
- Print damping, stiffness, plasticity state and rest length alongside results.
- Compare analytical equilibrium, not just whether a mass visually stops moving.
- Damping should affect oscillatory transient behaviour, not ideal static equilibrium while rest length is fixed.

## Suggested regression assertion

```cpp
// Illustrative test only: assumes you set up the above world and
// run it long enough to settle without plasticity.
const float expectedY = 1.9019f;
const float tolerance = 0.003f;
assert(std::abs(world.nodes[massNodeIndex].position.y - expectedY) < tolerance);
```

This illustrative assertion is not an existing project test. It shows how to automate the observation. See [[17 - Verification Tests and Exercises]].
