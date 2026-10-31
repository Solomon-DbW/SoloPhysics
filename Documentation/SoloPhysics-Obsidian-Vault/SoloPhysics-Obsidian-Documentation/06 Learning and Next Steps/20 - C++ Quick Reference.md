---
title: "20 — C++ Syntax Cheat Sheet for Python Programmers"
tags: [solophysics/documentation]
created: 2026-10-31
status: retrospective
---

# 20 — C++ Syntax Cheat Sheet for Python Programmers

## Read this before opening `Renderer.cpp`

| C++ notation | Plain English | Python rough analogy |
|---|---|---|
| `#include <file>` | make declarations from header available | `import` (compile-time, not runtime) |
| `#pragma once` | don't include same header twice in translation unit | no direct equivalent |
| `namespace solophysics` | scope for project symbols | module/namespace |
| `::` | qualify a scope | `module.name` |
| `.member` | member of a value/reference | `obj.attr` |
| `ptr->member` | member through a pointer | `obj.attr` after pointer dereference |
| `&` after a type | reference / alias | implicit object reference, not identical |
| `&x` expression | take address of x | no everyday equivalent |
| `*ptr` expression | dereference pointer | no everyday equivalent |
| `*opt` | retrieve contained optional value | unwrap `Optional` |
| `const T&` | read-only borrowed alias | immutable-access view analogy |
| `T&&` | rvalue reference | advanced, not yet needed |
| `auto` | compiler infers a concrete type | inferred typing, but static |
| `constexpr` | compile-time value when possible | `Final` isn't exact equivalent |
| `std::vector<T>` | dynamically sized contiguous sequence | `list[T]` |
| `std::array<T,N>` | fixed-size array | fixed-length tuple analogy |
| `std::size_t` | unsigned size/index type | `int` for indices |
| `std::optional<T>` | maybe T; maybe no value | `T | None` |
| `std::function<R(Args...)>` | stored callable with signature | `Callable[[Args], R]` |
| `[](...) {...}` | lambda, no captures | `lambda` (but C++ allows block body) |
| `[&](...) {...}` | lambda captures used locals by reference | closure captures variable bindings |
| `[=](...) {...}` | lambda captures used locals by value | closure capture snapshot analogy |
| `[this]` | capture current object pointer | bound method analogy |
| `for (auto& x : xs)` | iterate by reference | `for x in xs:` |
| `for (i=0; i<n; ++i)` | index loop | `for i in range(n):` |
| `if (!flag)` | if not true | `if not flag:` |
| `&&`, `||` | logical AND, OR | `and`, `or` |
| `true`, `false` | boolean values | `True`, `False` |
| `nullptr` | null pointer | `None` (not the same type) |
| `{0,1,2}` | aggregate/brace initialisation | list/tuple literal analogy |
| `std::cout << x` | write stdout | `print(x)` |
| `std::cerr << x` | write stderr | `print(x, file=sys.stderr)` |
| `throw` / `catch` | exception handling | `raise` / `except` |
| `static_cast<T>(x)` | explicit conversion | `T(x)` (not exact) |
| `sizeof(T)` | number of bytes for C++ object type | `struct.calcsize` analogy |
| `offsetof(T,member)` | member byte offset | C ABI `ctypes` analogy |
| `R"TAG(...)TAG"` | raw string literal | `r"..."` but with delimiter |
| `~Renderer()` | destructor | `__del__` analogy, but timing differs |
| `= delete` | disable a function/operation | no direct equivalent |

## Special vector syntax

```cpp
using Vector3D = glm::vec3;
Vector3D p{1.0f, 2.0f, 3.0f};
Vector3D v{0.0f};
Vector3D moved = p + v * 0.1f;
float distance = glm::length(moved - p);
float alignedSpeed = glm::dot(v, glm::normalize(p));
```

These operations are built by GLM's overloaded operators, analogous to NumPy array arithmetic—not automatic features of built-in C arrays.

## Common traps

- `=` assigns; `==` compares.
- `&` is reference in a declaration but address-of in many expressions.
- `*` multiplies numbers and dereferences pointers/optional values depending on context.
- `[]` is indexing for a container and capture syntax for a lambda.
- `std::vector` references can be invalidated by reallocation.
- `float` and `double` are not Python's single ordinary float type; literals ending in `f` are `float`.
- Always check the *first* compiler error before the cascade that follows.

## Further reading

[[03 - C++ for a Python Developer]] • [[04 - Data Model and Source Architecture]] • [[13 - Modern OpenGL and GLSL]] • [LearnCpp](https://www.learncpp.com/) • [cppreference](https://en.cppreference.com/)
