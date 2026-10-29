#pragma once
#include <solophysics/Vector3D.hpp>

namespace solophysics {

struct Node {
    Vector3D position{0.0f};
    Vector3D velocity{0.0f};
    Vector3D force{0.0f};

    float mass = 1.0f;
    bool fixed = false;
};

}