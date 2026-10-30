
#include <solophysics/Renderer.hpp>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace solophysics {

Renderer::Renderer(int width, int height) {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW initialisation failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(
        width, height, "SoloPhysics", nullptr, nullptr
    );

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Window creation failed");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Scroll callback adjusts zoom.
    glfwSetWindowUserPointer(window, this);

    glfwSetScrollCallback(
        window,
        [](GLFWwindow* win, double, double yoffset) {
            auto* renderer = static_cast<Renderer*>(
                glfwGetWindowUserPointer(win)
            );

            if (renderer) {
                float factor = std::pow(
                    1.1f,
                    static_cast<float>(yoffset)
                );

                renderer->zoom = std::clamp(
                    renderer->zoom * factor,
                    0.2f,
                    10.0f
                );
            }
        }
    );
}

Renderer::~Renderer() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Renderer::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Renderer::pollEvents() {
    glfwPollEvents();

    if (keyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

bool Renderer::keyPressed(int key) const {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool Renderer::mousePressed(int button) const {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

Vector3D Renderer::mouseWorldPosition() const {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    int windowWidth, windowHeight;
    int framebufferWidth, framebufferHeight;

    glfwGetWindowSize(
        window, &windowWidth, &windowHeight
    );

    glfwGetFramebufferSize(
        window, &framebufferWidth, &framebufferHeight
    );

    if (windowWidth <= 0 || windowHeight <= 0 ||
        framebufferWidth <= 0 || framebufferHeight <= 0) {
        return Vector3D{cameraX, cameraY, 0.0f};
    }

    float aspect =
        static_cast<float>(framebufferWidth) /
        static_cast<float>(framebufferHeight);

    float halfHeight = 2.5f / zoom;
    float halfWidth = halfHeight * aspect;

    float normalizedX =
        static_cast<float>(mx / windowWidth);

    float normalizedY =
        static_cast<float>(my / windowHeight);

    float worldX =
        cameraX + (normalizedX * 2.0f - 1.0f) * halfWidth;

    float worldY =
        cameraY + (1.0f - normalizedY * 2.0f) * halfHeight;

    return Vector3D{worldX, worldY, 0.0f};
}

void Renderer::updateCamera() {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    bool middlePressed =
        mousePressed(GLFW_MOUSE_BUTTON_MIDDLE);

    if (middlePressed && wasMiddlePressed) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        int windowWidth, windowHeight;
        glfwGetWindowSize(
            window, &windowWidth, &windowHeight
        );

        if (width > 0 && height > 0 &&
            windowWidth > 0 && windowHeight > 0) {

            float aspect =
                static_cast<float>(width) /
                static_cast<float>(height);

            float worldHeight = 5.0f / zoom;
            float worldWidth = worldHeight * aspect;

            cameraX -= static_cast<float>(
                (mx - previousMouseX) *
                worldWidth / windowWidth
            );

            cameraY += static_cast<float>(
                (my - previousMouseY) *
                worldHeight / windowHeight
            );
        }
    }

    previousMouseX = mx;
    previousMouseY = my;
    wasMiddlePressed = middlePressed;
}

void Renderer::updateProjection(int width, int height) {
    float aspect =
        static_cast<float>(width) /
        static_cast<float>(height);

    float halfHeight = 2.5f / zoom;
    float halfWidth = halfHeight * aspect;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(
        cameraX - halfWidth,
        cameraX + halfWidth,
        cameraY - halfHeight,
        cameraY + halfHeight,
        -1.0,
        1.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer::drawGround(const PhysicsWorld& world) {
    glColor3f(0.6f, 0.65f, 0.7f);
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    glVertex2f(-100.0f, world.groundY);
    glVertex2f(100.0f, world.groundY);
    glEnd();
}

void Renderer::drawBeams(const PhysicsWorld& world) {
    glLineWidth(3.0f);
    glBegin(GL_LINES);

    for (const Beam& beam : world.beams) {
        if (beam.broken) {
            continue;
        }

        const Node& a = world.nodes[beam.nodeA];
        const Node& b = world.nodes[beam.nodeB];

        float length = glm::length(
            b.position - a.position
        );

        float strain =
            (length - beam.restLength) / beam.restLength;

        if (strain < -0.01f) {
            glColor3f(0.2f, 0.5f, 1.0f);
        } else if (strain > 0.01f) {
            glColor3f(1.0f, 0.3f, 0.3f);
        } else {
            glColor3f(0.3f, 0.9f, 0.5f);
        }

        glVertex2f(a.position.x, a.position.y);
        glVertex2f(b.position.x, b.position.y);
    }

    glEnd();
}

void Renderer::drawNodes(const PhysicsWorld& world) {
    glPointSize(12.0f);
    glColor3f(1.0f, 0.9f, 0.3f);

    glBegin(GL_POINTS);

    for (const Node& node : world.nodes) {
        glVertex2f(node.position.x, node.position.y);
    }

    glEnd();
}

void Renderer::render(const PhysicsWorld& world) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (width <= 0 || height <= 0) {
        return;
    }

    glViewport(0, 0, width, height);

    glClearColor(0.055f, 0.075f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    updateProjection(width, height);

    drawGround(world);
    drawBeams(world);
    drawNodes(world);

    glfwSwapBuffers(window);
}

} // namespace solophysics
