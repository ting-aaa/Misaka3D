#include "SphereGeometry.h"

#include <cmath>

namespace Misaka {

SphereGeometry::SphereGeometry(float radius, int sectors, int stacks) {
    sectors = sectors < 3 ? 3 : sectors;
    stacks = stacks < 2 ? 2 : stacks;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const float pi = 3.14159265358979323846f;

    for (int i = 0; i <= stacks; ++i) {
        float v = static_cast<float>(i) / static_cast<float>(stacks);
        float stackAngle = pi * 0.5f - v * pi;
        float xy = radius * std::cos(stackAngle);
        float z = radius * std::sin(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float u = static_cast<float>(j) / static_cast<float>(sectors);
            float sectorAngle = u * 2.0f * pi;

            float x = xy * std::cos(sectorAngle);
            float y = xy * std::sin(sectorAngle);

            glm::vec3 n = glm::normalize(glm::vec3(x, y, z));

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);

            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    auto tangentVertices = ComputeTangents(vertices, indices);
    UploadGPU(tangentVertices, indices);
}

} // namespace Misaka
