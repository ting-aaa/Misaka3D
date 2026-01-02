#pragma once

#include "LightBase.h"

namespace Misaka {

class PointLight : public LightBase {
public:
    float constant = 1.0f;
    float linear;
    float quadratic;
    float radius;

    PointLight();

    void SetRange(float r);
    glm::vec3 GetPosition();
};

} // namespace Misaka
