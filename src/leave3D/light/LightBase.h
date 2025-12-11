#pragma once

#include "../ObjectContainer3D.h"

enum class LightType {
    Directional,
    Point,
    Spot
};

class LightBase: public ObjectContainer3D {

protected:
    LightType _lightType;

public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
    bool enabled = true;

    LightBase(LightType type) 
        : _lightType(type), color(glm::vec3(1.0,1.0,1.0)), intensity(1.0f), enabled(true) {}

    virtual ~LightBase() = default;

    LightType GetLightType() const {
        return _lightType;
    }
};