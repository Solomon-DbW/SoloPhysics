---
title: "13 — Modern OpenGL, GPU Resources and GLSL"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 13 — Modern OpenGL, GPU Resources and GLSL

## Why the OpenGL 3.3 change matters

OpenGL 2.1 compatibility mode allowed simple immediate-mode calls such as `glBegin`. OpenGL 3.3 **core** instead requires defining shaders and vertex input explicitly. The GPU draws the data you upload into buffers. The current v0.4 renderer in the supplied bundle uses **GLAD 2** to obtain function pointers and GLFW to create/context-manage the window.

## Two shader stages

Actual vertex-shader logic in the supplied source:

```glsl
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;
uniform mat4 uMVP;
out vec3 vColour;
void main() {
    gl_Position = uMVP * vec4(aPosition, 1.0);
    vColour = aColour;
}
```

`layout(location = 0)` binds attribute index 0 to a 3-component vertex position. `uniform` means one matrix value used across the draw call; `out` passes interpolated data to the next shader stage. `vec4(aPosition,1.0)` adds homogeneous coordinate $w=1$ so translations can be represented by a matrix multiplication.

Actual fragment-shader logic:

```glsl
#version 330 core
in vec3 vColour;
out vec4 FragColour;
uniform bool uRoundPoints;
void main() {
    if (uRoundPoints) {
        vec2 p = gl_PointCoord * 2.0 - 1.0;
        if (dot(p, p) > 1.0) discard;
    }
    FragColour = vec4(vColour, 1.0);
}
```

For points, `gl_PointCoord` gives a coordinate across the point primitive. Pixels outside a unit circle are discarded, creating round point sprites. `vec4(...,1.0)` assigns fully opaque alpha.

## Compile and link shaders

```cpp
GLuint shader = glCreateShader(type);
glShaderSource(shader, 1, &source, nullptr);
glCompileShader(shader);
glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
```

A shader **compiles** separately, then vertex and fragment shaders are **linked** into one shader program. The project checks compile/link status and throws an error containing the GPU driver's log on failure. This differs from compiling C++ files with `g++`: GLSL is compiled by the graphics driver at runtime.

## The packed vertex layout

```cpp
struct Vertex {
    Vector3D position;
    Vector3D colour;
};
```

`vao` (vertex array object) stores how vertex attributes are interpreted. `vbo` (vertex buffer object) owns the GPU-side byte buffer containing vertex data. The code declares the positions/colours to the GPU:

```cpp
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
glEnableVertexAttribArray(0);
```

- First argument `0`: GLSL attribute location 0.
- `3`: three scalar components per vertex.
- `GL_FLOAT`: underlying scalar data is `float`.
- `GL_FALSE`: don't normalise float values.
- `sizeof(Vertex)`: stride, in bytes, between consecutive vertices.
- `offsetof(...)`: byte offset from start of structure to member.
- `reinterpret_cast<void*>`: expresses the byte offset in the API's expected pointer-typed argument.

**Portability caveat:** for robust production code, ensure the `Vertex` layout matches the buffer attribute definition, including GLM alignment configuration. A future mesh format should consider explicit tightly packed fields or static layout assertions.

## Per-frame rendering

The renderer assembles a dynamic `std::vector<Vertex>` for ground lines, active beams, broken beams, mouse spring and points. For each group, the local `drawVertices` lambda uploads current vertices:

```cpp
glBufferData(GL_ARRAY_BUFFER,
    static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
    vertices.data(), GL_DYNAMIC_DRAW);
glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));
```

`vertices.data()` is a pointer to contiguous C++ vector memory. OpenGL copies data into the currently bound VBO. The `GL_DYNAMIC_DRAW` usage hint indicates the data is expected to change frequently. This is functional for 28 beams; at larger scales, batch allocations and persistent buffers may become more efficient.

## Why OpenGL cleanup matters

The renderer's destructor calls `glDeleteBuffers`, `glDeleteVertexArrays`, `glDeleteProgram`, destroys the GLFW window, then terminates GLFW. It makes the context current before deleting GL resources. This is **RAII** in practice: graphics resources have a controlled lifetime, instead of relying on a Python GC finalizer.

## Render order and depth

The ground grid is drawn with depth testing. Beams are depth-tested as lines. The source temporarily disables depth testing when drawing node points so they remain easy to locate. That **intentionally lets rear nodes remain visible through front geometry** for interaction and can lead to ambiguous selections: a useful debug feature but not a production occlusion policy.

## Future graphics improvements

Static grid VBO, more deliberate GPU buffer allocation, solid mesh skinning over the node cage, picking with depth, scalable vertex colours/strain gradients, cameras, optional ImGui inspector, and consistent frame pacing.

Sources: [LearnOpenGL Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle), [LearnOpenGL Camera](https://learnopengl.com/Getting-started/Camera), [GLFW quick guide](https://www.glfw.org/docs/latest/quick_guide.html).
