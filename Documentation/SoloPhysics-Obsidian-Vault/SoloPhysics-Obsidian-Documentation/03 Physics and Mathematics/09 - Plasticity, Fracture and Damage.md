---
title: "09 — Plastic Deformation, Fracture and Damage Flags"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 09 — Plastic Deformation, Fracture and Damage Flags

## The three regimes

1. **Elastic:** beam stretches/compresses then returns toward its rest length when the load disappears.
2. **Plastic:** beam's rest length itself changes; deformation persists when forces stop.
3. **Fractured:** beam is marked broken and its force contribution is skipped thereafter.

SoloPhysics implements a simplified **strain-threshold** model. For current length $L$ and current rest length $L_0$,

$$\varepsilon=\frac{L-L_0}{L_0},\qquad \varepsilon_{abs}=|\varepsilon|.$$

When $|\varepsilon|\ge\varepsilon_{break}$, `beam.broken=true`. Otherwise, if plastic deformation is enabled and $|\varepsilon|>\varepsilon_y$, the code gradually moves $L_0$ toward a yield-boundary length.

## Recorded plastic update

```cpp
float factor = std::clamp(beam.plasticity * dt, 0.0f, 1.0f);
float yieldBoundary = beam.restLength *
    (1.0f + std::copysign(beam.yieldStrain, strain));
float excess = length - yieldBoundary;
float oldRestLength = beam.restLength;
beam.restLength += excess * factor;
beam.restLength = std::max(beam.restLength, 1e-6f);
if (std::abs(beam.restLength - oldRestLength) > 1e-7f)
    beam.plasticallyDeformed = true;
```

This is a simplification recorded during the earlier 2D implementation; verify the newest `PhysicsWorld.cpp` before changing it. `std::copysign` keeps tension and compression yield boundaries on the appropriate side of zero strain. `std::clamp` limits how far the rest length can move during one step.

## Important limitation: moving strain reference

Because strain is divided by **the updated rest length**, yielding can reduce current strain and can postpone fracture during slow pulling. A more rigorous model should distinguish:

$$\varepsilon_{total}=\frac{L-L_{original}}{L_{original}},\qquad
\varepsilon_{elastic}=\frac{L-L_{plastic\,rest}}{L_{plastic\,rest}}$$

and track a history variable for accumulated plastic strain, damage or dissipated plastic work. A stress/force-based yield model and material calibration would be better for vehicle physics.

## Actual v0.4 demonstration parameters

From the checked 3D `src/main.cpp` bundle:

| Variable | Demo setting | Purpose |
|---|---:|---|
| `stiffness` | 650 N/m | Elastic axial response |
| `damping` | 12 N·s/m | Axial viscous damping |
| `yieldStrain` | 0.06 | Plastic update triggers above 6% |
| `breakStrain` | 0.30 | Beam fractures above 30% current-rest strain |
| `plasticity` | 0.6 | Rest-length adaptation rate |
| `mouseStiffness` | 500 N/m | User's virtual drag spring |
| `mouseDamping` | 35 N·s/m | Drag force damper |
| `maxMouseForce` | 1500 N | Clamp for large mouse pulls |

These **aren't validated properties of steel, aluminium or vehicle parts**; they were chosen for an interactive demo.

## Colours and state priority

The GLAD renderer uses green for near rest length, blue for compression, red for extension, orange for a plastically deformed beam, and grey for broken. Plastic history takes priority over current elastic strain (so an orange beam can also currently be stretched or compressed). Broken beams are still drawn as grey straight lines between endpoints **for debugging**, not as dangling fragments.

## First damage debugging incident

At one point, the square visibly responded to dragging but never yielded or broke because `main.cpp` still contained:

```cpp
beam.plasticDeformationEnabled = false;
beam.breakStrain = 100.0f;
```

The fix was to enable yielding, set feasible demonstration thresholds, anchor one node to force actual structural strain, and print `Plastic beams`/`Broken beams` alongside maximum strain. The user later confirmed the complete interactive damage test worked.

See [[16 - Errors, Diagnostics and Lessons]] and [[18 - Limits and Future Roadmap]].
