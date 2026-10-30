
#include <iostream>
#include <solophysics/PhysicsWorld.hpp>

int main() {
    solophysics::PhysicsWorld world;

    // Fixed node at 3 metres
    solophysics::Node anchor;
    anchor.position = {0.0f, 3.0f, 0.0f};
    anchor.fixed = true;

    // Movable node at 2 metres
    solophysics::Node mass;
    mass.position = {0.0f, 2.0f, 0.0f};
    mass.mass = 1.0f;

    // Add both nodes
    auto a = world.addNode(anchor);
    auto b = world.addNode(mass);

    // Connect them with a spring
    world.addBeam(a, b, 100.0f, 0.6f);

    // Disable plastic deformation
    world.beams[0].plasticDeformationEnabled = false;

    // Disable beam breaking during testing
    world.beams[0].breakStrain = 100.0f;

    std::cout
        << "Spring stiffness: " << world.beams[0].stiffness
        << "\nSpring damping: " << world.beams[0].damping
        << "\nPlasticity enabled: "
        << world.beams[0].plasticDeformationEnabled
        << '\n';

    constexpr float dt = 1.0f / 120.0f;

    // Simulate five seconds
    for (int i = 0; i < 600; ++i) {
        world.step(dt);

        // Print every 30 physics steps
        if (i % 30 == 0) {
            const auto& node = world.nodes[b];

            std::cout
                << "Time: " << (i + 1) * dt
                << " | Height: " << node.position.y
                << " | Velocity: " << node.velocity.y
                << " | Rest length: " << world.beams[0].restLength
                << " | Broken: " << world.beams[0].broken
                << '\n';
        }
    }

    return 0;
}
