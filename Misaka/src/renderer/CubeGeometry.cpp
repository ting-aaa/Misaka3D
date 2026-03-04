#include "CubeGeometry.h"

namespace Misaka {

CubeGeometry::CubeGeometry(float size) {
    const float h = size * 0.5f;

    std::vector<float> vertices = {
        -h, -h,  h,  0,  0,  1,  0, 0,
         h, -h,  h,  0,  0,  1,  1, 0,
         h,  h,  h,  0,  0,  1,  1, 1,
        -h,  h,  h,  0,  0,  1,  0, 1,

        -h, -h, -h,  0,  0, -1,  1, 0,
        -h,  h, -h,  0,  0, -1,  1, 1,
         h,  h, -h,  0,  0, -1,  0, 1,
         h, -h, -h,  0,  0, -1,  0, 0,

        -h, -h, -h, -1,  0,  0,  0, 0,
        -h, -h,  h, -1,  0,  0,  1, 0,
        -h,  h,  h, -1,  0,  0,  1, 1,
        -h,  h, -h, -1,  0,  0,  0, 1,

         h, -h, -h,  1,  0,  0,  1, 0,
         h,  h, -h,  1,  0,  0,  1, 1,
         h,  h,  h,  1,  0,  0,  0, 1,
         h, -h,  h,  1,  0,  0,  0, 0,

        -h,  h, -h,  0,  1,  0,  0, 1,
        -h,  h,  h,  0,  1,  0,  0, 0,
         h,  h,  h,  0,  1,  0,  1, 0,
         h,  h, -h,  0,  1,  0,  1, 1,

        -h, -h, -h,  0, -1,  0,  1, 1,
         h, -h, -h,  0, -1,  0,  0, 1,
         h, -h,  h,  0, -1,  0,  0, 0,
        -h, -h,  h,  0, -1,  0,  1, 0
    };

    std::vector<unsigned int> indices = {
         0,  1,  2,  2,  3,  0,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    auto tangentVertices = ComputeTangents(vertices, indices);
    UploadGPU(tangentVertices, indices);
}

} // namespace Misaka
