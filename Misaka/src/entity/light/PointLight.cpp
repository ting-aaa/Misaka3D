#include "PointLight.h"

namespace Misaka {

PointLight::PointLight()
    : LightBase(LightType::Point), linear(0.09f), quadratic(0.032f), radius(10.0f) {}

void PointLight::SetRange(float r) {
    radius = r;
    linear = 4.5f / r;
    quadratic = 75.0f / (r * r);
}

glm::vec3 PointLight::GetPosition() {
    glm::mat4 worldMatrix = GetWorldMatrix();
    return glm::vec3(worldMatrix[3]);
}

} // namespace Misaka
