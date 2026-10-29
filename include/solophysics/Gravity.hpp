#pragma once

#include <solophysics/Node.hpp>

namespace solophysics {

inline void applyGravity(Node& node) {          // 'inline' eliminates 'multiple definitions of applyGravity()' when including Gravity.hpp in multiple files
    if (node.fixed) {
        return;
    }

    const Vector3D gravity{0.0f, -9.81f, 0.0f};

    node.force += node.mass * gravity;
}

}
