
#pragma once

#include <solophysics/PhysicsWorld.hpp>

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
    void render(const PhysicsWorld& world);

    bool keyPressed(int key) const;
    bool mousePressed(int button) const;

    Vector3D mouseWorldPosition() const;

    void updateCamera();

private:
    GLFWwindow* window = nullptr;

    float cameraX = 0.0f;
    float cameraY = 2.0f;
    float zoom = 1.0f;

    double previousMouseX = 0.0;
    double previousMouseY = 0.0;

    bool wasMiddlePressed = false;

    void drawGround(const PhysicsWorld& world);
    void drawBeams(const PhysicsWorld& world);
    void drawNodes(const PhysicsWorld& world);

    void updateProjection(int width, int height);
};

} // namespace solophysics
