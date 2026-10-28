// include/solophysics/Node.hpp
#pragma once

#include <glm/vec3.hpp>

namespace solophysics {

struct Node {
    glm::vec3 position{0.0f};
    glm::vec3 previousPosition{0.0f};
    glm::vec3 velocity{0.0f};
    glm::vec3 force{0.0f};

    float mass = 1.0f;
    bool fixed = false;
};

}
