#include <solophysics/PhysicsWorld.hpp>

#include <glm/geometric.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace solophysics {

    void PhysicsWorld::resetForces() {
        for (Node& node : nodes) {
            node.force = Vector3D{0.0f};
        }
    }

    void PhysicsWorld::applyExternalForces() {
        for (Node& node : nodes) {
            if (node.fixed) {
                continue;
            }

            node.force += node.mass * gravity;
        }
    }

void PhysicsWorld::solveBeamForces() {
    for (const Beam& beam : beams) {
        if (beam.broken) {
            continue;
        }

        Node& a = nodes[beam.nodeA];
        Node& b = nodes[beam.nodeB];

        Vector3D delta = b.position - a.position;
        float length = glm::length(delta);

        if (length < 1e-6f) {
            continue;
        }

        Vector3D direction = delta / length;

        float extension = length - beam.restLength;

        float relativeVelocity = glm::dot(
            b.velocity - a.velocity,
            direction
        );

        float springForce = beam.stiffness * extension;
        float dampingForce = beam.damping * relativeVelocity;

        Vector3D force =
            (springForce + dampingForce) * direction;

        if (!a.fixed) {
            a.force += force;
        }

        if (!b.fixed) {
            b.force -= force;
        }
    }
}

void PhysicsWorld::integrate(float dt) {
    for (Node& node : nodes) {
        if (node.fixed) {
            continue;
        }

        Vector3D acceleration = node.force / node.mass;

        node.velocity += acceleration * dt;
        node.position += node.velocity * dt;
    }
}


    void PhysicsWorld::detectAndResolveGroundContacts() {
        constexpr float restingVelocityThreshold = 0.2f;

        for (Node& node : nodes) {
            if (node.fixed) {
                continue;
            }

            if (node.position.y < groundY) {
                // Move the node back onto the ground
                node.position.y = groundY;

                if (node.velocity.y < 0.0f) {
                    float impactSpeed = -node.velocity.y;

                    if (impactSpeed < restingVelocityThreshold) {
                        // Small impacts: treat as resting contact
                        node.velocity.y = 0.0f;
                    } else {
                        // Larger impacts: apply restitution
                        node.velocity.y *= -restitution;
                    }
                }

                // Simplified horizontal friction
                node.velocity.x *= (1.0f - friction);
                node.velocity.z *= (1.0f - friction);
            }
        }
    }



    void PhysicsWorld::updateDamage(float dt) {
        for (Beam& beam : beams) {
            if (beam.broken) {
                continue;
            }

            const Node& a = nodes[beam.nodeA];
            const Node& b = nodes[beam.nodeB];

            float length = glm::length(
                b.position - a.position
            );

            if (beam.restLength <= 1e-6f) {
                continue;
            }

            float strain =
                (length - beam.restLength) / beam.restLength;

            float absoluteStrain = std::abs(strain);

            // 1. Check whether the beam should break
            if (absoluteStrain >= beam.breakStrain) {
                beam.broken = true;
                continue;
            }

            // 2. Apply plastic deformation only if enabled
            if (beam.plasticDeformationEnabled &&
                absoluteStrain > beam.yieldStrain) {

                float factor = std::clamp(
                    beam.plasticity * dt,
                    0.0f,
                    1.0f
                );

                float yieldBoundary =
                    beam.restLength *
                    (1.0f + std::copysign(
                        beam.yieldStrain, strain
                    ));

                float excess = length - yieldBoundary;

                beam.restLength += factor * excess;

                beam.restLength =
                    std::max(beam.restLength, 1e-6f);
            }
        }
    }


    void PhysicsWorld::validateState() const {
        for (const Node& node : nodes) {
            if (!std::isfinite(node.mass) ||
                node.mass <= 0.0f) {
                throw std::runtime_error(
                    "Invalid node mass"
                );
            }

            for (int axis = 0; axis < 3; ++axis) {
                if (!std::isfinite(node.position[axis]) ||
                    !std::isfinite(node.velocity[axis]) ||
                    !std::isfinite(node.force[axis])) {
                    throw std::runtime_error(
                        "Non-finite physics state"
                    );
                }
            }
        }

        for (const Beam& beam : beams) {
            if (beam.nodeA >= nodes.size() ||
                beam.nodeB >= nodes.size()) {
                throw std::runtime_error(
                    "Beam references invalid node"
                );
            }

            if (!std::isfinite(beam.restLength) ||
                beam.restLength <= 0.0f) {
                throw std::runtime_error(
                    "Invalid beam rest length"
                );
            }
        }
    }

    void PhysicsWorld::step(float dt) {
        if (!std::isfinite(dt) || dt <= 0.0f) {
            throw std::invalid_argument(
                "Physics timestep must be positive and finite"
            );
        }

        validateState();

        // 1. Reset force accumulators
        resetForces();

        // 2. Apply gravity
        applyExternalForces();

        // 3. Calculate spring and damping forces
        solveBeamForces();

        // 4. Integrate velocity and position
        integrate(dt);

        // 5 & 6. Detect and resolve ground collisions
        detectAndResolveGroundContacts();

        // 7. Update structural damage
        updateDamage(dt);

        // 8. Check simulation stability
        validateState();
    }

    std::size_t PhysicsWorld::addNode(const Node& node) {
        if (!std::isfinite(node.mass) || node.mass <= 0.0f) {
            throw std::invalid_argument("Node mass must be positive");
        }

        nodes.push_back(node);
        return nodes.size() - 1;
    }

    void PhysicsWorld::addBeam(
        std::size_t a,
        std::size_t b,
        float stiffness,
        float damping
    ) {
        if (a >= nodes.size() || b >= nodes.size() || a == b) {
            throw std::invalid_argument("Invalid beam endpoints");
        }

        if (!std::isfinite(stiffness) || stiffness < 0.0f ||
            !std::isfinite(damping) || damping < 0.0f) {
            throw std::invalid_argument("Invalid beam parameters");
        }

        float length = glm::length(
            nodes[b].position - nodes[a].position
        );

        if (!std::isfinite(length) || length <= 1e-6f) {
            throw std::invalid_argument(
                "Beam endpoints must be separated"
            );
        }

        Beam beam;
        beam.nodeA = a;
        beam.nodeB = b;
        beam.restLength = length;
        beam.stiffness = stiffness;
        beam.damping = damping;

        beams.push_back(beam);
    }

} // namespace solophysics

