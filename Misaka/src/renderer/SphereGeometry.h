#pragma once

#include "Geometry.h"

namespace Misaka {

class SphereGeometry : public Geometry {
public:
    SphereGeometry(float radius = 0.5f, int sectors = 32, int stacks = 16);
};

} // namespace Misaka
