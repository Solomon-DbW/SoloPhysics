---
title: "10 — The First 2D GLFW/OpenGL Renderer"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 10 — The First 2D GLFW/OpenGL Renderer

## Why add graphics only after headless tests?

The CLI square-drop already produced useful positions and velocities. A renderer made the dynamics understandable visually, but it shouldn't change the physics results. The first GLFW version received `const PhysicsWorld&` and used an **orthographic XY projection** of a 3D physics state.

## Basic GLFW lifecycle

```cpp
if (!glfwInit()) throw std::runtime_error("GLFW init failed");
GLFWwindow* window = glfwCreateWindow(1000, 700, "SoloPhysics", nullptr, nullptr);
if (!window) throw std::runtime_error("Window creation failed");
glfwMakeContextCurrent(window);
glfwSwapInterval(1);
while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    // step physics and draw
    glfwSwapBuffers(window);
}
glfwDestroyWindow(window);
glfwTerminate();
```

This is an illustrative lifecycle excerpt, **not** the complete current code. GLFW creates native windows and OpenGL contexts; it does not itself implement the physics engine or GPU shaders. `glfwSwapInterval(1)` requests vsync; actual frame rate depends on environment.

## Legacy OpenGL prototype

The original `Renderer.cpp` used a requested **OpenGL 2.1 compatibility context**, `glBegin(GL_LINES)`, `glVertex2f(...)`, `glEnd()`, `glMatrixMode`, and `glOrtho`. This was an intentionally short teaching step, not the final architecture. Each frame drew:

- ground line;
- all unbroken beams as coloured segments;
- nodes as points.

```cpp
glBegin(GL_LINES);
for (const Beam& beam : world.beams) {
    if (beam.broken) continue;
    const Node& a = world.nodes[beam.nodeA];
    const Node& b = world.nodes[beam.nodeB];
    glVertex2f(a.position.x, a.position.y);
    glVertex2f(b.position.x, b.position.y);
}
glEnd();
```

**Syntax:** `glBegin` and `glEnd` are function calls. `GL_LINES` is a named API constant. The `for (const Beam& beam : ...)` loop accesses each beam by read-only reference. The renderer reads node positions and translates them into screen geometry.

## The early screen-space/world-space trap

World position $x,y$ is measured in metres; mouse location is measured in window pixels. A screen-centred orthographic camera with vertical span $H$ converts cursor coordinates approximately as:

$$x_w=x_c+\left(2\frac{x_p}{W}-1\right)\frac{H}{2}\frac{W}{H_{pixel}},$$

$$y_w=y_c+\left(1-2\frac{y_p}{H_{pixel}}\right)\frac{H}{2}.$$

The actual prototype also handled framebuffer size separately from window size for scaling on HiDPI setups. Its zoom and panning affected the mapping.

## Why replace the renderer?

Modern OpenGL core profiles intentionally omit legacy immediate-mode drawing calls. Moving to OpenGL 3.3 shaders and vertex buffers makes the engine more portable to modern contexts and introduces the GPU pipeline required for larger meshes and later vehicle rendering.

See [[13 - Modern OpenGL and GLSL]] and [GLFW's official getting-started guide](https://www.glfw.org/docs/latest/quick_guide.html).
