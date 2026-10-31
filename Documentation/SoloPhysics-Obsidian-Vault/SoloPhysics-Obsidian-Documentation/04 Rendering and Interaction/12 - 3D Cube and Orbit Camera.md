---
title: "12 — From Square to 3D Cube and Orbit Camera"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 12 — From Square to 3D Cube and Orbit Camera

## The v0.4 geometry

The actual supplied v0.4 `main.cpp` creates **8 nodes**, four at $y=2$ m and four at $y=3$ m, with $x,z\in\{-0.5,+0.5\}$ m. Node 0 starts fixed. Gravity starts off, so the user can pull the structure without free fall; pressing `F` releases the anchor, and `G` enables gravity.

The edges form a fully connected 8-node network, $K_8$, with

$$\binom{8}{2}=\frac{8\times7}{2}=28\text{ unique connections}.$$

The source enumerates 12 cube edges, 12 face diagonals and 4 body diagonals. This is deliberately heavily braced; it is not a realistic vehicle-body finite element model.

```cpp
const std::array<Vector3D, 8> positions{{
    {-0.5f, 2.0f, -0.5f},
    { 0.5f, 2.0f, -0.5f},
    { 0.5f, 2.0f,  0.5f},
    {-0.5f, 2.0f,  0.5f},
    {-0.5f, 3.0f, -0.5f},
    { 0.5f, 3.0f, -0.5f},
    { 0.5f, 3.0f,  0.5f},
    {-0.5f, 3.0f,  0.5f}
}};
```

`std::array<Vector3D,8>` has exactly eight GLM vectors. The *two sets* of braces are common in `std::array` aggregate initialisation and should not be confused with a Python nested tuple of arbitrary size.

## Orbit camera geometry

The camera stores a **target** (point to look at), a **yaw** (left/right rotation), a **pitch** (up/down rotation), and a **distance**. Given camera target $\mathbf t$, distance $r$, yaw $\theta$ and pitch $\phi$:

$$\mathbf x_{camera}=\mathbf t+r\begin{bmatrix}
\cos\phi\sin\theta\\
\sin\phi\\
\cos\phi\cos\theta
\end{bmatrix}.$$

The renderer calls `glm::lookAt(cameraPosition(), cameraTarget, {0,1,0})` to obtain a view matrix. Right-drag changes yaw/pitch, middle-drag translates the target, scrolling scales distance (clamped between 1.5 and 40 in the supplied version).

## From world coordinates to GPU pixels

A 3D point in homogeneous coordinates $\mathbf p=(x,y,z,1)$ passes through

$$\boxed{\mathbf p_{clip}=PVM\mathbf p_{model}}.$$

Current app geometry is directly in world coordinates, so its model transform $M$ is effectively identity and the code computes `mvp = projectionMatrix() * viewMatrix()`. The matrix product order matters: for column-vector convention, view acts first and projection acts second.

The current projection is a perspective camera:

```cpp
glm::perspective(glm::radians(55.0f), aspect, 0.05f, 100.0f);
```

`55°` is vertical field of view; `aspect = framebufferWidth/framebufferHeight`; `0.05` m and `100` m set near/far clipping planes. Keeping the near plane above zero avoids an invalid perspective projection.

## Screen-space picking and unprojection

- `glm::project`: world point → framebuffer X/Y/depth.
- `glm::unProject`: framebuffer X/Y/depth → world point.
- `cursorRay()`: two unprojected endpoints → a ray.
- `beginDrag()`: record a point and a camera-facing plane normal.
- `mouseDragTarget()`: ray/plane intersection.

This is **selection by projected point radius**, not mesh triangle ray-intersection. It is appropriate for a node-based prototype but would need occlusion, depth sorting and hit tests for complex visible geometry.

## Scope of 3D collisions

The existing solver checks node height versus a flat $y=groundY$ plane. The renderer draws a **grid on the XZ plane**; the grid does not itself simulate collision. The simple ground solver handles point contacts, not edge/triangle collisions, contact patches, angular friction or full cube–cube intersection.

See [[13 - Modern OpenGL and GLSL]] and [[18 - Limits and Future Roadmap]].
