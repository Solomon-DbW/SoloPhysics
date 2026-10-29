#pragma once

#include <cstddef>

namespace solophysics {

struct Beam {
    std::size_t nodeA = 0;
    std::size_t nodeB = 0;

    float restLength = 0.0f;     // Length of the beam at rest
    float stiffness = 1000.0f;   // Beam's spring constant (the 'k' in F = -kΔL) where 'F' = spring force along the beam
    float damping = 10.0f;       // Beam's damping coefficient (the 'c' in F = -cv) where F = damping force and v = relative velocity projected along the beam

    // Simplified material parameters for initial beam
    float yieldStrain = 0.15f;   // Beam's elastic limit
    float breakStrain = 0.60f;   // Beam's maximum tensile stress before it breaks
    float plasticity = 2.0f;

    bool broken = false;
};

}
