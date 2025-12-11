#pragma once

#include "LightBase.h"

class DirectionalLight : public LightBase {
public:
    DirectionalLight() : LightBase(LightType::Directional) {}
    ~DirectionalLight() override = default;

    glm::vec3 GetDirection() {
        glm::mat4 worldMaterix = GetWorldMatrix();
        return glm::normalize(glm::vec3(worldMaterix[2])); // Z 轴负方向
    }
};