---
title: "03 — C++ for a Python Developer"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 03 — C++ for a Python Developer

## 1. The biggest differences

C++ is statically typed, compiled ahead of execution, and exposes object lifetime, copies, references, pointers, stack/heap allocation and generic templates more directly than Python. The SoloPhysics code uses **C++20**, but many constructs predate C++20.

| Concept | Python | SoloPhysics C++ |
|---|---|---|
| Variable | `mass = 1.0` | `float mass = 1.0f;` |
| Constant-like binding | `DT = 1 / 240` (convention) | `constexpr double dt = 1.0 / 240.0;` |
| Container | `nodes = []` | `std::vector<Node> nodes;` |
| Fixed-size collection | `positions = (...)` | `std::array<Vector3D, 8> positions;` |
| Optional result | `None` or index | `std::optional<std::size_t>` |
| Function | `def step(dt):` | `void step(float dt);` |
| Exception | `raise ValueError(...)` | `throw std::runtime_error(...);` |
| Object attribute | `node.position` | `node.position` or `nodePtr->position` |
| Module organisation | `physics_world.py` | `PhysicsWorld.hpp` + `PhysicsWorld.cpp` |

> [!warning] Python analogy limitations
> Python names usually refer to objects; C++ values can be copied by assignment. `Node a = b;` copies `b`. `Node& a = b;` is an alias to `b`. `Node* p = &b;` is a pointer containing its address.

## 2. From source to symbols

```cpp
#include <glm/geometric.hpp>
namespace solophysics {
    using Vector3D = glm::vec3;
}
```

- `#include` tells the preprocessor to make declarations available.
- `namespace solophysics { ... }` groups symbols, rather like `solophysics.` names in Python.
- `::` is the **scope resolution operator**: `glm::vec3` refers to `vec3` in namespace `glm`.
- `using Vector3D = glm::vec3;` defines a type alias, **not a new class**.
- `;` terminates most statements. `{}` delimit blocks instead of Python indentation.

## 3. Values, references, const and pointers

```cpp
void inspect(const PhysicsWorld& world) {
    const Node& a = world.nodes[0];
    // a.mass = 5.0f; // ERROR: read-only reference
}

void accelerate(Node& node, const Vector3D& force) {
    node.force += force; // changes the original node
}
```

`PhysicsWorld& world` means **reference** (alias, no copy). `const PhysicsWorld&` makes the accessed object read-only from this function. `Node* node` means **pointer**. `&node` in an expression takes an address, and `*ptr` dereferences a pointer. The `*` in `std::optional` dereferences the contained value too, but `optional` is not itself a pointer.

```cpp
GLFWwindow* window = nullptr;
glfwMakeContextCurrent(window);
```

`nullptr` is the null-pointer literal; GLFW manages a window behind this pointer. **Never** dereference a null pointer. The renderer destroys the GLFW window during cleanup.

## 4. Aggregates and brace initialisation

```cpp
Node topLeft;
topLeft.position = {-0.5f, 3.0f, 0.0f};
topLeft.mass = 1.0f;
```

`3.0f` is a `float` literal, while `3.0` is usually a `double`. `{x, y, z}` supplies elements/constructor inputs to a GLM vector. `Vector3D{0.0f}` constructs a zero vector (as used in the project).

## 5. Iteration and index types

```cpp
for (Beam& beam : world.beams) {
    beam.breakStrain = 0.30f;
}

for (std::size_t i = 0; i < world.nodes.size(); ++i) {
    const Node& node = world.nodes[i];
}
```

The first is a **range-based `for`** (like Python's `for beam in beams`); `&` avoids copying each `Beam` and permits mutation. The second is an indexed loop; `std::size_t` is an unsigned size/index type, `++i` increments it, `world.nodes[i]` indexes the vector. The `i` variable exists **only inside the loop**—the exact scope error that appeared in the project's 2D mouse-picking code.

## 6. Automatic type deduction, templates and structured bindings

```cpp
const std::array<std::pair<std::size_t, std::size_t>, 28> connections{{
    {0, 1}, {1, 2} /* ... other entries ... */
}};

for (const auto& [a, b] : connections) {
    world.addBeam(a, b, stiffness, damping);
}
```

- `std::array<T, N>`: fixed-size sequence with `N` elements of type `T`.
- `std::pair<A,B>`: two-value aggregate, like a typed 2-tuple.
- `auto`: compiler infers the type; unlike Python, the type is still static.
- `const auto& [a, b]`: structured binding, unpacking the pair **without copying the pair**.
- `<...>`: template arguments, not a less-than comparison in this context.

## 7. Lambdas and capture syntax

```cpp
auto createWorld = []() {
    PhysicsWorld world;
    return world;
};

auto applyMouseForce = [&](PhysicsWorld& physics) {
    if (!grabbedNode) return;
    Node& node = physics.nodes[*grabbedNode];
    node.force += mouseStiffness * (mouseTarget - node.position)
                - mouseDamping * node.velocity;
};
```

A lambda is an anonymous function (Python `lambda` analogy, but C++ lambdas may contain multiple statements). `[]` is the capture list. Empty `[]`: use no outside local variables. `[&]`: capture used enclosing local variables by **reference**. The function body begins after `{`. `return` indicates the result; a `void` lambda has no returned value. The user's callback is stored in `std::function<void(PhysicsWorld&)>` and invoked from the physics timestep. **Lifetime rule:** captured references must remain valid for as long as the callback uses them.

## 8. Optional values

```cpp
std::optional<std::size_t> grabbedNode;
if (grabbedNode) {
    Node& node = world.nodes[*grabbedNode];
}
grabbedNode.reset();
```

`std::optional<T>` is an explicit 'value or no value' wrapper, similar to `int | None` in Python typing. `if (grabbedNode)` checks **presence**, not whether the index is nonzero; index `0` is valid. `*grabbedNode` retrieves it only after checking presence. `.reset()` returns to the empty state.

## 9. Classes, public/private, constructors and cleanup

```cpp
class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();
    Renderer(const Renderer&) = delete;
private:
    GLFWwindow* window = nullptr;
};
```

- `class` defaults to private members, `struct` defaults to public members.
- `public:` exposes methods to callers; `private:` hides implementation details.
- `Renderer(...)` is a **constructor**, called on object creation.
- `~Renderer()` is a **destructor**, called during destruction.
- `= delete` prohibits copying the renderer; two copies owning one GLFW window could double-free GPU/window resources.
- RAII (*Resource Acquisition Is Initialization*) ties cleanup to object lifetime. Python has analogous context managers (`with`), but destruction timing differs.

## 10. Error handling and ownership

```cpp
try {
    Renderer renderer(1200, 800);
    // application loop
} catch (const std::exception& e) {
    std::cerr << "SoloPhysics error: " << e.what() << '\n';
    return 1;
}
```

The `catch` catches standard-library exception types derived from `std::exception`. `std::cerr` is the error output stream; `<<` inserts text into the stream. Objects with automatic lifetime clean up as the exception propagates. Constructors must themselves clean up resources they acquired if construction throws before completion.

## 11. Syntax you will meet in the renderer

| Syntax | Meaning |
|---|---|
| `GLenum` / `GLuint` | OpenGL API-defined integer-like types |
| `sizeof(Vertex)` | Byte size of a vertex structure |
| `offsetof(Vertex, colour)` | Byte offset of member `colour` inside `Vertex` |
| `reinterpret_cast<void*>(...)` | Explicit low-level cast for the OpenGL vertex offset API |
| `R"GLSL( ... )GLSL"` | Raw string literal containing shader source |
| `const char*` | Pointer to a null-terminated character sequence |
| `static_cast<float>(x)` | Explicit checked-category conversion, e.g. `double` to `float` |
| `std::clamp(x, low, high)` | Bound value within interval |
| `std::nullopt` | Empty optional value |
| `#pragma once` | Widely supported header multiple-inclusion protection |
| `constexpr` | Compile-time evaluable constant when applicable |

## 12. Pointer/reference traps for Python developers

1. A reference cannot be reseated after binding. `Node& a = nodes[0]` will always refer to that node object during its lifetime.
2. `std::vector` can reallocate on growth, invalidating references/pointers to elements. This is why a beam stores indices, with explicit bounds checks.
3. `const` is enforced by the compiler in ordinary code; it isn't a runtime immutable proxy.
4. Ownership of C pointers to OpenGL/GLFW resources must be managed explicitly; don't assume Python-style garbage collection.
5. A `std::function` holding `[&]` captures can create dangling references if its lifetime exceeds those local variables.

See [[20 - C++ Quick Reference]] for an exam-style sheet and [[17 - Verification Tests and Exercises]] for practice.
