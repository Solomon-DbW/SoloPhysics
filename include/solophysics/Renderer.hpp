#pragma once

#include <solophysics/PhysicsWorld.hpp>

#include <glm/mat4x4.hpp>

#include <cstddef>
#include <optional>

struct GLFWwindow;

namespace solophysics {

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool shouldClose() const;
    void pollEvents();
    bool keyPressed(int key) const;
    bool mousePressed(int button) const;
    void setTitle(const char* title);

    // Right-click orbit, middle-click pan, scroll zoom.
    void updateCamera();
    std::optional<std::size_t> pickNode(
        const PhysicsWorld& world, float radiusPixels = 18.0f
    ) const;

    void beginDrag(const Vector3D& nodePosition);
    std::optional<Vector3D> mouseDragTarget() const;
    void endDrag();

    void render(
        const PhysicsWorld& world,
        std::optional<std::size_t> grabbedNode,
        std::optional<std::size_t> hoveredNode,
        const Vector3D& mouseTarget
    );

private:
    struct Ray {
        Vector3D origin;
        Vector3D direction;
    };

    GLFWwindow* window = nullptr;
    unsigned int program = 0;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    int mvpLocation = -1;
    int roundPointsLocation = -1;

    Vector3D cameraTarget{0.0f, 1.5f, 0.0f};
    float cameraYaw = 0.75f;
    float cameraPitch = 0.35f;
    float cameraDistance = 7.0f;

    double previousMouseX = 0.0;
    double previousMouseY = 0.0;
    bool previousRight = false;
    bool previousMiddle = false;

    bool dragging = false;
    Vector3D dragPlanePoint{0.0f};
    Vector3D dragPlaneNormal{0.0f, 0.0f, 1.0f};

    Vector3D cameraPosition() const;
    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix() const;
    std::optional<Ray> cursorRay() const;
};

} // namespace solophysics
