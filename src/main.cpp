
#include <solophysics/PhysicsWorld.hpp>
#include <solophysics/Renderer.hpp>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <limits>
#include <optional>

int main() {
    try {
        using namespace solophysics;

        auto createWorld = []() {
            PhysicsWorld world;

            world.groundY = 0.0f;
            world.restitution = 0.2f;
            world.friction = 0.4f;

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

            world.addBeam(n0, n1, stiffness, damping);
            world.addBeam(n1, n3, stiffness, damping);
            world.addBeam(n3, n2, stiffness, damping);
            world.addBeam(n2, n0, stiffness, damping);
            world.addBeam(n0, n3, stiffness, damping);
            world.addBeam(n1, n2, stiffness, damping);

            for (Beam& beam : world.beams) {
                beam.plasticDeformationEnabled = false;
                beam.breakStrain = 100.0f;
            }

            return world;
        };

        PhysicsWorld world = createWorld();
        Renderer renderer(1000, 700);

        bool paused = false;
        bool previousSpace = false;
        bool previousR = false;
        bool previousG = false;
        bool previousLeft = false;

        std::optional<std::size_t> grabbedNode;

        Vector3D mouseTarget{0.0f};

        // Mouse spring parameters
        constexpr float mouseStiffness = 500.0f;
        constexpr float mouseDamping = 30.0f;

        world.externalForceCallback =
            [&](PhysicsWorld& physics) {
                if (!grabbedNode) {
                    return;
                }

                Node& node = physics.nodes[*grabbedNode];

                if (node.fixed) {
                    return;
                }

                Vector3D displacement =
                    mouseTarget - node.position;

                Vector3D dragForce =
                    mouseStiffness * displacement -
                    mouseDamping * node.velocity;

                node.force += dragForce;
            };

        constexpr double fixedDt = 1.0 / 240.0;

        double previousTime = glfwGetTime();
        double accumulator = 0.0;

        while (!renderer.shouldClose()) {
            renderer.pollEvents();
            renderer.updateCamera();

            // Pause/resume
            bool space = renderer.keyPressed(GLFW_KEY_SPACE);

            if (space && !previousSpace) {
                paused = !paused;
            }

            previousSpace = space;

            // Reset
            bool r = renderer.keyPressed(GLFW_KEY_R);

            if (r && !previousR) {
                world = createWorld();
                grabbedNode.reset();

                world.externalForceCallback =
                    [&](PhysicsWorld& physics) {
                        if (!grabbedNode) {
                            return;
                        }

                        Node& node =
                            physics.nodes[*grabbedNode];

                        if (node.fixed) {
                            return;
                        }

                        Vector3D displacement =
                            mouseTarget - node.position;

                        node.force +=
                            mouseStiffness * displacement -
                            mouseDamping * node.velocity;
                    };

                accumulator = 0.0;
            }

            previousR = r;

            // Gravity toggle
            bool g = renderer.keyPressed(GLFW_KEY_G);

            if (g && !previousG) {
                if (glm::length(world.gravity) > 0.0f) {
                    world.gravity = {0.0f, 0.0f, 0.0f};
                } else {
                    world.gravity = {0.0f, -9.81f, 0.0f};
                }
            }

            previousG = g;

            // Mouse picking
            bool left = renderer.mousePressed(
                GLFW_MOUSE_BUTTON_LEFT
            );

            mouseTarget = renderer.mouseWorldPosition();

            if (left && !previousLeft) {
                std::cout << "Left mouse clicked\n";

                std::cout
                    << "Mouse world position: "
                    << mouseTarget.x << ", "
                    << mouseTarget.y << '\n';

                float closestDistance = 0.2f;
                            // float closestDistance = 0.2f;

                for (std::size_t i = 0;
                     i < world.nodes.size(); ++i) {

                    float distance = glm::distance(
                        world.nodes[i].position,
                        mouseTarget
                    );

                    if (distance < closestDistance) {
                        closestDistance = distance;
                        grabbedNode = i;
                    }
                }
            }

            if (!left) {
                grabbedNode.reset();
            }

            previousLeft = left;

            // Physics timing
            double currentTime = glfwGetTime();
            double frameTime = std::clamp(
                currentTime - previousTime,
                0.0,
                0.1
            );

            previousTime = currentTime;

            if (!paused) {
                accumulator += frameTime;

                while (accumulator >= fixedDt) {
                    world.step(
                        static_cast<float>(fixedDt)
                    );

                    accumulator -= fixedDt;
                }
            } else {
                accumulator = 0.0;
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
