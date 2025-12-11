#pragma once
#include "LightBase.h"

class PointLight : public LightBase {
public:
    /**衰减 */
    float constant = 1.0f;
    /**线性衰减 */
    float linear;
    /**二次衰减 */
    float quadratic;
    /**光照半径 */
    float radius;

    PointLight() :LightBase(LightType::Point), linear(0.09f), quadratic(0.032f), radius(10.0f) {}

    void SetRange(float r) {
        radius = r;
        linear = 4.5f / r;
        quadratic = 75.0f / (r * r);
    }

    glm::vec3 GetPosition() {
        glm::mat4 worldMaterix = GetWorldMatrix();
        return glm::vec3(worldMaterix[3]);
    }
};