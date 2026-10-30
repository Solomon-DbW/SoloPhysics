
#include <solophysics/Renderer.hpp>

#include <GLFW/glfw3.h>

#include <glm/geometric.hpp>

#include <stdexcept>
#include <string>
#include <cmath>

namespace solophysics {

Renderer::Renderer(int width, int height) {
    if (!glfwInit()) {
        throw std::runtime_error(
            "Failed to initialise GLFW"
        );
    }

    // Legacy OpenGL compatibility context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(
        width,
        height,
        "SoloPhysics",
        nullptr,
        nullptr
    );

    if (!window) {
        glfwTerminate();
        throw std::runtime_error(
            "Failed to create GLFW window"
        );
    }

    glfwMakeContextCurrent(window);

    // Enable vertical synchronisation
    glfwSwapInterval(1);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void Renderer::setTitle(const char* title) {
    glfwSetWindowTitle(window, title);
}

void Renderer::drawGround(const PhysicsWorld& world) {
    glColor3f(0.6f, 0.65f, 0.7f);
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    glVertex2f(-10.0f, world.groundY);
    glVertex2f(10.0f, world.groundY);
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

        float currentLength = glm::length(
            b.position - a.position
        );

        float strain = (
            currentLength - beam.restLength
        ) / beam.restLength;

        // Blue: compression
        // Green: near rest length
        // Red: extension
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
        glVertex2f(
            node.position.x,
            node.position.y
        );
    }

    glEnd();
}

void Renderer::render(const PhysicsWorld& world) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window, &width, &height);

    if (width <= 0 || height <= 0) {
        return;
    }

    glViewport(0, 0, width, height);

    glClearColor(0.055f, 0.075f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect =
        static_cast<float>(width) /
        static_cast<float>(height);

    // Keep world units proportional
    float halfHeight = 2.5f;
    float halfWidth = halfHeight * aspect;

    glOrtho(
        -halfWidth,
         halfWidth,
        -0.5,
         4.5,
        -1.0,
         1.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawGround(world);
    drawBeams(world);
    drawNodes(world);

    glfwSwapBuffers(window);
}

} // namespace solophysics
