#pragma once

#include <vector>                                 // Dynamic array yhat automatically resizes itself when elements are added or removed
#include <cstddef>

#include <solophysics/Node.hpp>
#include <solophysics/Beam.hpp>

#include <functional>

namespace solophysics {

class PhysicsWorld {

//__________________________________________________________PUBLIC CLASS FUNCTIONS__________________________________________________________

public:
    std::vector<Node> nodes;
    std::vector<Beam> beams;
    std::function<void(PhysicsWorld&)> externalForceCallback;

    Vector3D gravity{0.0f, -9.81f, 0.0f};          // Adds gravity to system

    float groundY = 0.0f;                          // Y-coordinates of the ground

    // Restitution quantifies the elasticity of a collision between objects, measuring the ratio of relative speed after impact to relative speed before impact
    float restitution = 0.1f;
    float friction = 0.4f;

    void step(float dt);

    std::size_t addNode(const Node& node);         // Adds Node object to system
    void addBeam(std::size_t a, std::size_t b,     // Adds Beam object to system
                 float stiffness, float damping);

//__________________________________________________________PRIVATE CLASS FUNCTIONS__________________________________________________________

private:
    void resetForces();
    void applyExternalForces();
    void solveBeamForces();
    void integrate(float dt);
    void detectAndResolveGroundContacts();
    void updateDamage(float dt);
    void validateState() const;
};

}
