
#include <solophysics/PhysicsWorld.hpp>
#include <solophysics/Renderer.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <exception>

int main() {
    try {
        using namespace solophysics;

        PhysicsWorld world;

        world.groundY = 0.0f;
        world.restitution = 0.2f;
        world.friction = 0.4f;

        // Create the square
        Node topLeft;
        topLeft.position = {-0.5f, 3.0f, 0.0f};

        Node topRight;
        topRight.position = {0.5f, 3.0f, 0.0f};

        Node bottomLeft;
        bottomLeft.position = {-0.5f, 2.0f, 0.0f};

        Node bottomRight;
        bottomRight.position = {0.5f, 2.0f, 0.0f};

        auto n0 = world.addNode(topLeft);
        auto n1 = world.addNode(topRight);
        auto n2 = world.addNode(bottomLeft);
        auto n3 = world.addNode(bottomRight);

        constexpr float stiffness = 1000.0f;
        constexpr float damping = 10.0f;

        // Edges
        world.addBeam(n0, n1, stiffness, damping);
        world.addBeam(n1, n3, stiffness, damping);
        world.addBeam(n3, n2, stiffness, damping);
        world.addBeam(n2, n0, stiffness, damping);

        // Diagonal braces
        world.addBeam(n0, n3, stiffness, damping);
        world.addBeam(n1, n2, stiffness, damping);

        // Elastic-only test
        for (Beam& beam : world.beams) {
            beam.plasticDeformationEnabled = false;
            beam.breakStrain = 100.0f;
        }

        Renderer renderer(1000, 700);

        constexpr double fixedDt = 1.0 / 240.0;

        double previousTime = glfwGetTime();
        double accumulator = 0.0;

        while (!renderer.shouldClose()) {
            renderer.pollEvents();

            double currentTime = glfwGetTime();
            double frameTime = currentTime - previousTime;
            previousTime = currentTime;

            // Prevent a large physics jump after a pause
            frameTime = std::clamp(
                frameTime, 0.0, 0.1
            );

            accumulator += frameTime;

            // Fixed timestep physics
            while (accumulator >= fixedDt) {
                world.step(
                    static_cast<float>(fixedDt)
                );

                accumulator -= fixedDt;
            }

            renderer.render(world);
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr
            << "SoloPhysics error: "
            << e.what()
            << '\n';

        return 1;
    }
}
