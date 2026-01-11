#pragma once

#include "LightBase.h"

namespace Misaka {

class DirectionalLight : public LightBase {
public:
    DirectionalLight();
    ~DirectionalLight() override = default;

    glm::vec3 GetDirection();
};

} // namespace Misaka
