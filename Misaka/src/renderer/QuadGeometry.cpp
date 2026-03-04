#include "QuadGeometry.h"

namespace Misaka {

QuadGeometry::QuadGeometry(float size) {
    const float h = size * 0.5f;
    std::vector<float> vertices = {
        -h, -h, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         h, -h, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         h,  h, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -h,  h, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    auto tangentVertices = ComputeTangents(vertices, indices);
    UploadGPU(tangentVertices, indices);
}

} // namespace Misaka
