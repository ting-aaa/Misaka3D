#include "DirectionalLight.h"

namespace Misaka {

DirectionalLight::DirectionalLight() : LightBase(LightType::Directional) {}

glm::vec3 DirectionalLight::GetDirection() {
    glm::mat4 worldMatrix = GetWorldMatrix();
    return glm::normalize(glm::vec3(worldMatrix[2]));
}

} // namespace Misaka
