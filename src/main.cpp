#include <solophysics/PhysicsWorld.hpp>
#include <solophysics/Renderer.hpp>

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <utility>

int main() {
    try {
        using namespace solophysics;

        // The core physics system remains 3D. Build a braced 8-node cube.
        auto createWorld = []() {
            PhysicsWorld world;
            world.groundY = 0.0f;
            world.restitution = 0.2f;
            world.friction = 0.4f;

            // Default to an anchored, gravity-free damage experiment.
            // Press F to unanchor and G to enable gravity for a drop test.
            world.gravity = {0.0f, 0.0f, 0.0f};

            const std::array<Vector3D, 8> positions{{
                {-0.5f, 2.0f, -0.5f}, // 0 bottom rear left (anchor)
                { 0.5f, 2.0f, -0.5f}, // 1 bottom rear right
                { 0.5f, 2.0f,  0.5f}, // 2 bottom front right
                {-0.5f, 2.0f,  0.5f}, // 3 bottom front left
                {-0.5f, 3.0f, -0.5f}, // 4 top rear left
                { 0.5f, 3.0f, -0.5f}, // 5 top rear right
                { 0.5f, 3.0f,  0.5f}, // 6 top front right
                {-0.5f, 3.0f,  0.5f}  // 7 top front left
            }};

            for (const auto& p : positions) {
                Node node;
                node.position = p;
                node.mass = 1.0f;
                world.addNode(node);
            }

            world.nodes[0].fixed = true;

            // 12 edges + 12 face diagonals + 4 body diagonals = 28 beams.
            const std::array<std::pair<std::size_t, std::size_t>, 28> connections{{
                {0,1}, {1,2}, {2,3}, {3,0},
                {4,5}, {5,6}, {6,7}, {7,4},
                {0,4}, {1,5}, {2,6}, {3,7},
                {0,2}, {1,3}, // bottom face
                {4,6}, {5,7}, // top face
                {0,5}, {1,4}, // rear face
                {3,6}, {2,7}, // front face
                {0,7}, {3,4}, // left face
                {1,6}, {2,5}, // right face
                {0,6}, {1,7}, {2,4}, {3,5} // body diagonals
            }};

            constexpr float stiffness = 650.0f;
            constexpr float damping = 12.0f;

            for (const auto& [a, b] : connections) {
                world.addBeam(a, b, stiffness, damping);
            }

            for (Beam& beam : world.beams) {
                beam.plasticDeformationEnabled = true;
                beam.yieldStrain = 0.06f;
                beam.breakStrain = 0.30f;
                beam.plasticity = 0.6f;
                beam.plasticallyDeformed = false;
                beam.broken = false;
            }

            return world;
        };

        PhysicsWorld world = createWorld();
        Renderer renderer(1200, 800);

        bool paused = false;
        bool previousSpace = false;
        bool previousR = false;
        bool previousG = false;
        bool previousF = false;
        bool previousLeft = false;

        std::optional<std::size_t> grabbedNode;
        Vector3D mouseTarget{0.0f};
        constexpr float mouseStiffness = 500.0f;
        constexpr float mouseDamping = 35.0f;

        // IMPORTANT: PhysicsWorld::step() must invoke this callback after
        // resetForces()/applyExternalForces() and before integrate().
        auto applyMouseForce = [&](PhysicsWorld& physics) {
            if (!grabbedNode || *grabbedNode >= physics.nodes.size()) return;
            Node& node = physics.nodes[*grabbedNode];
            if (node.fixed) return;

            Vector3D force = mouseStiffness * (mouseTarget - node.position)
                           - mouseDamping * node.velocity;

            // Protect the explicit spring integrator from extreme mouse pulls.
            constexpr float maxMouseForce = 1500.0f;
            const float magnitude = glm::length(force);
            if (magnitude > maxMouseForce) {
                force *= maxMouseForce / magnitude;
            }
            node.force += force;
        };

        world.externalForceCallback = applyMouseForce;

        constexpr double fixedDt = 1.0 / 240.0;
        double previousTime = glfwGetTime();
        double accumulator = 0.0;
        double lastTitleUpdate = previousTime;
        std::size_t steps = 0;

        std::cout << "SoloPhysics v0.4 - 3D soft-body damage test\n"
                  << "Nodes: " << world.nodes.size()
                  << " | Beams: " << world.beams.size() << "\n\n"
                  << "Left drag  : Grab node\n"
                  << "Right drag : Orbit camera\n"
                  << "Middle drag: Pan camera\n"
                  << "Wheel      : Zoom\n"
                  << "Space      : Pause / resume\n"
                  << "R          : Reset\n"
                  << "G          : Toggle gravity\n"
                  << "F          : Toggle fixed anchor (node 0)\n"
                  << "Esc        : Exit\n\n"
                  << "Start: anchored cube, gravity OFF. Press F, then G for a drop.\n";

        while (!renderer.shouldClose()) {
            renderer.pollEvents();
            renderer.updateCamera();

            bool space = renderer.keyPressed(GLFW_KEY_SPACE);
            if (space && !previousSpace) {
                paused = !paused;
                std::cout << (paused ? "Paused\n" : "Resumed\n");
            }
            previousSpace = space;

            bool reset = renderer.keyPressed(GLFW_KEY_R);
            if (reset && !previousR) {
                world = createWorld();
                world.externalForceCallback = applyMouseForce;
                grabbedNode.reset();
                renderer.endDrag();
                steps = 0;
                accumulator = 0.0;
                paused = false;
                std::cout << "Simulation reset\n";
            }
            previousR = reset;

            bool gravityKey = renderer.keyPressed(GLFW_KEY_G);
            if (gravityKey && !previousG) {
                const bool enabled = glm::length(world.gravity) > 0.0f;
                world.gravity = enabled ? Vector3D{0.0f, 0.0f, 0.0f}
                                        : Vector3D{0.0f, -9.81f, 0.0f};
                std::cout << "Gravity " << (enabled ? "OFF\n" : "ON\n");
            }
            previousG = gravityKey;

            bool anchorKey = renderer.keyPressed(GLFW_KEY_F);
            if (anchorKey && !previousF) {
                Node& anchor = world.nodes[0];
                anchor.fixed = !anchor.fixed;
                if (anchor.fixed) anchor.velocity = Vector3D{0.0f};
                if (grabbedNode && *grabbedNode == 0) {
                    grabbedNode.reset();
                    renderer.endDrag();
                }
                std::cout << "Anchor " << (anchor.fixed ? "ON\n" : "OFF\n");
            }
            previousF = anchorKey;

            // Select nodes in screen pixels, not world-space distance.
            const bool left = renderer.mousePressed(GLFW_MOUSE_BUTTON_LEFT);
            auto hoveredNode = renderer.pickNode(world, 19.0f);
            if (left && !previousLeft) {
                grabbedNode = hoveredNode;
                if (grabbedNode && world.nodes[*grabbedNode].fixed) {
                    grabbedNode.reset();
                }
                if (grabbedNode) {
                    renderer.beginDrag(world.nodes[*grabbedNode].position);
                    mouseTarget = world.nodes[*grabbedNode].position;
                    std::cout << "Grabbed node " << *grabbedNode << '\n';
                }
            }
            if (!left) {
                if (previousLeft && grabbedNode) {
                    std::cout << "Released node " << *grabbedNode << '\n';
                }
                grabbedNode.reset();
                renderer.endDrag();
            }
            previousLeft = left;

            if (grabbedNode) {
                if (auto target = renderer.mouseDragTarget()) mouseTarget = *target;
            }

            double now = glfwGetTime();
            double frameTime = std::clamp(now - previousTime, 0.0, 0.1);
            previousTime = now;

            if (!paused) {
                accumulator += frameTime;
                while (accumulator >= fixedDt) {
                    world.step(static_cast<float>(fixedDt));
                    accumulator -= fixedDt;
                    ++steps;
                }
            } else {
                accumulator = 0.0;
            }

            if (now - lastTitleUpdate >= 0.25) {
                std::size_t plastic = 0;
                std::size_t broken = 0;
                for (const Beam& beam : world.beams) {
                    if (beam.plasticallyDeformed) ++plastic;
                    if (beam.broken) ++broken;
                }
                std::ostringstream title;
                title << "SoloPhysics 3D | " << (paused ? "PAUSED" : "RUNNING")
                      << " | Gravity " << (glm::length(world.gravity) > 0.0f ? "ON" : "OFF")
                      << " | Plastic " << plastic << '/' << world.beams.size()
                      << " | Broken " << broken << '/' << world.beams.size();
                renderer.setTitle(title.str().c_str());
                lastTitleUpdate = now;
            }

            renderer.render(world, grabbedNode, hoveredNode, mouseTarget);
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "SoloPhysics error: " << e.what() << '\n';
        return 1;
    }
}
