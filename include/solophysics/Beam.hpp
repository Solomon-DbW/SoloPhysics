// include/solophysics/Beam.hpp
#pragma once

#include <cstddef>

namespace solophysics {

struct Beam {
    std::size_t nodeA = 0;
    std::size_t nodeB = 0;

    float restLength = 0.0f;
    float stiffness = 1000.0f;
    float damping = 10.0f;

    float yieldForce = 5000.0f;
    float breakForce = 10000.0f;

    bool broken = false;
};

}
