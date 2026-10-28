# CMake

+ Build system generator
+ Project and its dependencies are described in `CMakeLists.txt`
+ CMake generates build instructions
+ Ninja uses those instructions to compile the code

---

### CMake version

```cpp
cmake_minimum_required(VERSION 3.23)
```
+ Specifies that the project needs `CMake 3.23` or later
+ CMake uses this to determine whuch features and policies are available

---

### Project declaration

```cpp
project(SoloPhysics
    VERSION 0.1.0
    LANGUAGES CXX
)
```
+ Sets the project name as `SoloPhysics`
+ Sets initial version as `0.1.0`
+ Tells CMake that the project uses `C++`

---

## C++ standard

```cpp
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

+ Selects `C++20` as the language standard
+ Requires the compiler to support the requested standard
+ Requests standard C++ rather than compiler-specific language extensions

---

## Finding GLM

```cpp
find_package(glm CONFIG REQUIRED)
```

+ `CONFIG` asks CMake to use the package's configuration files
+ `REQUIRED` makes configuration fail if GLM cannot be found

---

## Creating an executable

```cpp
add_executable(solo_physics
    src/main.cpp
)
```

+ Tells CMake to compile `src/main.cpp`
+ Links the resulting object code into an executable named `solo_physics`

---

## Header search paths

```cpp
target_include_directories(solo_physics
    PRIVATE include
)
```

+ Tells the compiler where to find the header files
+ `PRIVATE` means this include directory applies to the target being configured, rather than automatically becoming a public requirement for targets that link to it

---

## Linking GLM

```cpp
target_link_libraries(solo_physics
    PRIVATE glm::glm
)
```

+ Attaches GLM's CMake target to your executable
+ This generally supplies the required include paths and configuration rather than linking a conventional compiled GLM binary

---

## Compiler warnings

```cpp
target_compile_options(solo_physics
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
```

+ Enable useful compiler warnings
+ E.g. the compiler may warn about an unused variable, suspicious conversions or questionable language constructs
+ Warnings are particularly useful in physics code because a small mistake in types, units or calculations can lead to unstable simulations that are difficult to diagnose

---

## Build and run

```cpp
cmake -S . -B build -G Ninja
cmake --build build
./build/solo_physics
```

+ `-S .`: use the current directory as the source directory
+ `-B build`: put the generated build files in `build/` directory
+ `-G Ninja`: generate Ninja build instructions
+ `cmake --build build`: compile the project using the selected build system
+ `./build/solo_physics`: runs the executable
