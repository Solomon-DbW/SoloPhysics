#include <iostream>
#include <glm/vec3.hpp>

int main() {
    glm::vec3 position{0.0f, 5.0f, 0.0f}; // 3D vector of (0x, 5y, 0z)

    std::cout << "SoloPhysics v0.1.0\n";
    std::cout << "Initial height: "
              << position.y << " metres\n";

    return 0;
}
