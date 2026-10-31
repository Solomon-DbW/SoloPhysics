---
title: "11 — Interactive Controls, Picking and Mouse Springs"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 11 — Interactive Controls, Picking and Mouse Springs

## The problem: force accumulators were reset

The first drag attempt failed because the application added forces at the wrong point in the update sequence. Each `PhysicsWorld::step()` begins by setting all `node.force` fields to zero. If `main.cpp` writes `node.force += ...` **before** `step()`, that force disappears on the very next reset.

**Solution:** `PhysicsWorld` exposes an `externalForceCallback` invoked **after** resetting forces and applying gravity but **before** solving beams and integrating.

```cpp
// Recorded interface
std::function<void(PhysicsWorld&)> externalForceCallback;

// In step() (illustrative excerpt)
resetForces();
applyExternalForces();
if (externalForceCallback) externalForceCallback(*this);
solveBeamForces();
integrate(dt);
```

`std::function<void(PhysicsWorld&)>` means: an object callable with one mutable `PhysicsWorld&` and returning `void`. Python analogy: a stored callable attribute such as `world.external_force_callback = callback`.

## The virtual mouse spring

$$\boxed{\mathbf F_{mouse}=k_m(\mathbf x_{target}-\mathbf x_{node})-c_m\mathbf v_{node}}.$$

The current 3D source contains:

```cpp
auto applyMouseForce = [&](PhysicsWorld& physics) {
    if (!grabbedNode || *grabbedNode >= physics.nodes.size()) return;
    Node& node = physics.nodes[*grabbedNode];
    if (node.fixed) return;
    Vector3D force = mouseStiffness * (mouseTarget - node.position)
                   - mouseDamping * node.velocity;
    constexpr float maxMouseForce = 1500.0f;
    float magnitude = glm::length(force);
    if (magnitude > maxMouseForce)
        force *= maxMouseForce / magnitude;
    node.force += force;
};
```

This code **does not teleport a node**; it applies a bounded spring force, allowing the beam network to respond. `node.force +=` adds to rather than replaces gravity and other external forces.

## Picking: from metres to pixels

The original 2D picking selected the nearest node inside a fixed world-space radius (initially 0.2 m, later enlarged for diagnosis). This felt inconsistent as zoom changed.

The current 3D renderer uses `glm::project(world.nodes[i].position, viewMatrix(), projectionMatrix(), viewport)` to transform each node into framebuffer coordinates. It compares distance to the mouse cursor **in pixels** (19 px in the supplied v0.4 caller), so the selection radius is consistent with screen scale.

```cpp
auto hoveredNode = renderer.pickNode(world, 19.0f);
if (left && !previousLeft) {
    grabbedNode = hoveredNode;
    if (grabbedNode && world.nodes[*grabbedNode].fixed)
        grabbedNode.reset();
    if (grabbedNode)
        renderer.beginDrag(world.nodes[*grabbedNode].position);
}
```

`left && !previousLeft` is an **edge detector**: act once when the button transitions from released to pressed. Without it, selection could restart every frame while held.

## 3D dragging is not a 2D screen coordinate

In 3D, the cursor represents a **ray** extending out from the camera. The program unprojects the near/far cursor points, forms a ray, then intersects it with a plane passing through the selected node and facing the camera:

$$\mathbf r(t)=\mathbf o+t\mathbf d, \quad \mathbf n\cdot(\mathbf r(t)-\mathbf p_0)=0,$$

$$\boxed{t=\frac{\mathbf n\cdot(\mathbf p_0-\mathbf o)}{\mathbf n\cdot\mathbf d}}.$$

If the denominator is close to zero, the ray is parallel to the plane; return no target (`std::nullopt`). This gives intuitive movement approximately parallel to the screen at the node's initial depth. It does not implement arbitrary depth-axis gizmos yet.

## UI feedback

- Normal node: yellow; fixed node: purple.
- Hovered: cyan; grabbed: orange.
- A yellowish drag-spring line joins grabbed node and cursor target.
- `Space`, `R`, `G` and later `F` toggle pause, reset, gravity and the anchor; `Esc` exits.

**Debugging lesson:** seeing `Left mouse clicked` confirmed GLFW, but wasn't proof that node picking succeeded; only seeing selected IDs and applying forces confirmed the chain.

Further reading: [GLFW Input Guide](https://www.glfw.org/docs/latest/input.html). See [[03 - C++ for a Python Developer]] for lambdas and optional values.
