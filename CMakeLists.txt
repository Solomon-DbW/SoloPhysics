
cmake_minimum_required(VERSION 3.23)

project(SoloPhysics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(solo_physics
    src/main.cpp
    src/physics/PhysicsWorld.cpp
    src/rendering/Renderer.cpp
)

target_include_directories(solo_physics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(solo_physics PRIVATE
    glm::glm
    glfw
    OpenGL::GL
)

target_compile_options(solo_physics PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
