
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
    void setTitle(const char* title);

private:
    GLFWwindow* window = nullptr;

    void drawGround(const PhysicsWorld& world);
    void drawBeams(const PhysicsWorld& world);
    void drawNodes(const PhysicsWorld& world);
};

} // namespace solophysics
