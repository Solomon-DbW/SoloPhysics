
#include <solophysics/PhysicsWorld.hpp>
#include <solophysics/Renderer.hpp>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>

int main() {
    try {
        using namespace solophysics;

        // =====================================================
        // CREATE PHYSICS WORLD
        // =====================================================

        auto createWorld = []() {
            PhysicsWorld world;

            world.groundY = 0.0f;
            world.restitution = 0.2f;
            world.friction = 0.4f;

            // Disable gravity for the structural damage test
            world.gravity = Vector3D{0.0f, 0.0f, 0.0f};

            // Square nodes
            Node topLeft;
            topLeft.position = {-0.5f, 3.0f, 0.0f};

            Node topRight;
            topRight.position = {0.5f, 3.0f, 0.0f};

            Node bottomLeft;
            bottomLeft.position = {-0.5f, 2.0f, 0.0f};

            Node bottomRight;
            bottomRight.position = {0.5f, 2.0f, 0.0f};

            // Add nodes
            auto n0 = world.addNode(topLeft);
            auto n1 = world.addNode(topRight);
            auto n2 = world.addNode(bottomLeft);
            auto n3 = world.addNode(bottomRight);

            // Anchor the bottom-left node
            world.nodes[n2].fixed = true;

            // Beam properties
            constexpr float stiffness = 1000.0f;
            constexpr float damping = 10.0f;

            // Square edges
            world.addBeam(n0, n1, stiffness, damping);
            world.addBeam(n1, n3, stiffness, damping);
            world.addBeam(n3, n2, stiffness, damping);
            world.addBeam(n2, n0, stiffness, damping);

            // Diagonal supports
            world.addBeam(n0, n3, stiffness, damping);
            world.addBeam(n1, n2, stiffness, damping);

            // Enable plastic deformation and fracture
            for (Beam& beam : world.beams) {
                beam.plasticDeformationEnabled = true;

                beam.yieldStrain = 0.02f;
                beam.breakStrain = 0.12f;
                beam.plasticity = 0.5f;

                beam.plasticallyDeformed = false;
                beam.broken = false;
            }

            return world;
        };

        // =====================================================
        // INITIALISE WORLD AND RENDERER
        // =====================================================

        PhysicsWorld world = createWorld();
        Renderer renderer(1000, 700);

        // =====================================================
        // INPUT STATE
        // =====================================================

        bool paused = false;

        bool previousSpace = false;
        bool previousR = false;
        bool previousG = false;
        bool previousLeft = false;

        std::optional<std::size_t> grabbedNode;
        std::optional<std::size_t> hoveredNode;

        Vector3D mouseTarget{0.0f};

        // Mouse dragging spring
        constexpr float mouseStiffness = 500.0f;
        constexpr float mouseDamping = 30.0f;

        // =====================================================
        // MOUSE FORCE CALLBACK
        // =====================================================

        auto applyMouseForce = [&](PhysicsWorld& physics) {
            if (!grabbedNode) {
                return;
            }

            if (*grabbedNode >= physics.nodes.size()) {
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

        world.externalForceCallback = applyMouseForce;

        // =====================================================
        // FIXED TIMESTEP
        // =====================================================

        constexpr double fixedDt = 1.0 / 240.0;

        double previousTime = glfwGetTime();
        double accumulator = 0.0;

        std::size_t physicsSteps = 0;

        std::cout << "SoloPhysics - Structural Damage Test\n";
        std::cout << "Nodes: " << world.nodes.size()
                  << " | Beams: " << world.beams.size()
                  << '\n';

        std::cout << "\nControls:\n";
        std::cout << "Space        - Pause/resume\n";
        std::cout << "R            - Reset\n";
        std::cout << "G            - Toggle gravity\n";
        std::cout << "Left mouse   - Drag node\n";
        std::cout << "Middle mouse - Pan camera\n";
        std::cout << "Mouse wheel  - Zoom\n";
        std::cout << "Esc          - Exit\n\n";

        // =====================================================
        // MAIN APPLICATION LOOP
        // =====================================================

        while (!renderer.shouldClose()) {
            renderer.pollEvents();
            renderer.updateCamera();

            // =================================================
            // PAUSE / RESUME
            // =================================================

            bool space = renderer.keyPressed(GLFW_KEY_SPACE);

            if (space && !previousSpace) {
                paused = !paused;

                std::cout
                    << (paused ? "Paused\n" : "Resumed\n");
            }

            previousSpace = space;

            // =================================================
            // RESET SIMULATION
            // =================================================

            bool r = renderer.keyPressed(GLFW_KEY_R);

            if (r && !previousR) {
                world = createWorld();

                // Restore mouse force callback after reset
                world.externalForceCallback = applyMouseForce;

                grabbedNode.reset();
                hoveredNode.reset();

                accumulator = 0.0;
                physicsSteps = 0;
                paused = false;

                std::cout << "Simulation reset\n";
            }

            previousR = r;

            // =================================================
            // TOGGLE GRAVITY
            // =================================================

            bool g = renderer.keyPressed(GLFW_KEY_G);

            if (g && !previousG) {
                if (glm::length(world.gravity) > 0.0f) {
                    world.gravity =
                        Vector3D{0.0f, 0.0f, 0.0f};

                    std::cout << "Gravity disabled\n";
                } else {
                    world.gravity =
                        Vector3D{0.0f, -9.81f, 0.0f};

                    std::cout << "Gravity enabled\n";
                }
            }

            previousG = g;

            // =================================================
            // MOUSE POSITION
            // =================================================

            bool left = renderer.mousePressed(
                GLFW_MOUSE_BUTTON_LEFT
            );

            mouseTarget = renderer.mouseWorldPosition();

            // =================================================
            // HOVERED NODE DETECTION
            // =================================================

            hoveredNode.reset();

            float hoverDistance = 0.5f;

            for (std::size_t i = 0;
                 i < world.nodes.size();
                 ++i) {

                float distance = glm::distance(
                    world.nodes[i].position,
                    mouseTarget
                );

                if (distance < hoverDistance) {
                    hoverDistance = distance;
                    hoveredNode = i;
                }
            }

            // =================================================
            // NODE PICKING
            // =================================================

            if (left && !previousLeft) {
                grabbedNode.reset();

                float closestDistance = 0.5f;

                for (std::size_t i = 0;
                     i < world.nodes.size();
                     ++i) {

                    // Fixed nodes cannot be dragged
                    if (world.nodes[i].fixed) {
                        continue;
                    }

                    float distance = glm::distance(
                        world.nodes[i].position,
                        mouseTarget
                    );

                    if (distance < closestDistance) {
                        closestDistance = distance;
                        grabbedNode = i;
                    }
                }

                if (grabbedNode) {
                    std::cout
                        << "Grabbed node: "
                        << *grabbedNode
                        << '\n';
                }
            }

            // Release dragged node
            if (!left && previousLeft) {
                if (grabbedNode) {
                    std::cout
                        << "Released node: "
                        << *grabbedNode
                        << '\n';
                }

                grabbedNode.reset();
            }

            previousLeft = left;

            // =================================================
            // PHYSICS TIMING
            // =================================================

            double currentTime = glfwGetTime();

            double frameTime = std::clamp(
                currentTime - previousTime,
                0.0,
                0.1
            );

            previousTime = currentTime;

            if (!paused) {
                accumulator += frameTime;

                // =============================================
                // FIXED PHYSICS UPDATE
                // =============================================

                while (accumulator >= fixedDt) {
                    world.step(
                        static_cast<float>(fixedDt)
                    );

                    accumulator -= fixedDt;
                    ++physicsSteps;

                    // =========================================
                    // DAMAGE DIAGNOSTICS
                    // =========================================

                    // Print every 0.5 seconds of simulation time
                    if (physicsSteps % 120 == 0) {
                        int plasticCount = 0;
                        int brokenCount = 0;

                        float maximumStrain = 0.0f;

                        for (const Beam& beam : world.beams) {
                            if (beam.plasticallyDeformed) {
                                ++plasticCount;
                            }

                            if (beam.broken) {
                                ++brokenCount;
                            }

                            const Node& a =
                                world.nodes[beam.nodeA];

                            const Node& b =
                                world.nodes[beam.nodeB];

                            float currentLength = glm::length(
                                b.position - a.position
                            );

                            float strain = std::abs(
                                (currentLength - beam.restLength) /
                                beam.restLength
                            );

                            maximumStrain = std::max(
                                maximumStrain,
                                strain
                            );
                        }

                        std::cout
                            << "Time: "
                            << physicsSteps * fixedDt
                            << "s"
                            << " | Max strain: "
                            << maximumStrain * 100.0f
                            << "%"
                            << " | Plastic beams: "
                            << plasticCount
                            << " | Broken beams: "
                            << brokenCount
                            << '\n';
                    }
                }
            } else {
                // Prevent accumulated simulation time
                // from building up while paused
                accumulator = 0.0;
            }

            // =================================================
            // RENDER WORLD
            // =================================================

            renderer.render(
                world,
                grabbedNode,
                hoveredNode,
                mouseTarget
            );
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
