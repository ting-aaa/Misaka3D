#pragma once

#include <glm/glm.hpp>
#include "../ObjectContainer3D.h"

namespace Misaka {

enum class LightType {
    Directional,
    Point,
    Spot
};

class LightBase : public ObjectContainer3D {
protected:
    LightType _lightType;

public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
    bool enabled = true;

    LightBase(LightType type);
    virtual ~LightBase() = default;

    LightType GetLightType() const;
};

} // namespace Misaka
