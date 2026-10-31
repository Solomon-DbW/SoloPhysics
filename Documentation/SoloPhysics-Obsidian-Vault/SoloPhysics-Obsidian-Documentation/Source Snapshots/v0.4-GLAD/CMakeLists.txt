cmake_minimum_required(VERSION 3.23)
project(SoloPhysics LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# GLAD 2 source generator. No GLEW, GLX or X11-specific loader is required.
# CMake downloads GLAD once during first configuration.
include(FetchContent)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG 658f48e72aee3c6582e80b05ac0f8787a64fe6bb # GLAD v2.0.6
    SOURCE_SUBDIR cmake
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(glad)

# Generate a small OpenGL 3.3 core-profile function loader using the
# Khronos spec bundled with GLAD, without requesting external extensions.
glad_add_library(glad_gl_core_33 STATIC REPRODUCIBLE API gl:core=3.3 EXTENSIONS NONE)

add_executable(solo_physics
    src/main.cpp
    src/physics/PhysicsWorld.cpp
    src/rendering/Renderer.cpp
)

target_include_directories(solo_physics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# GLFW must not introduce its system OpenGL header in any C++ translation unit.
target_compile_definitions(solo_physics PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(solo_physics PRIVATE
    glm::glm
    glfw
    glad_gl_core_33
)

target_compile_options(solo_physics PRIVATE
    -Wall -Wextra -Wpedantic
)
