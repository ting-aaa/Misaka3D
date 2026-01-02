#pragma once

#include "MaterialBase.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Misaka {

class CommonMaterial : public MaterialBase {
public:
    // 1. 基础颜色 (Albedo)
    unsigned int albedoMap = 0;
    glm::vec4 color = glm::vec4(1.0f);

    // 2. 法线 (Normal)
    unsigned int normalMap = 0;

    // 3. 粗糙度 (Roughness)
    unsigned int roughnessMap = 0;
    float roughness = 0.5f;

    // 4. 金属度 (Metallic)
    unsigned int metallicMap = 0;
    float metallic = 0.0f;

    // 光照方向
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.5f, 1.0f, 0.5f));

    CommonMaterial(std::shared_ptr<Shader> shader);

    void uploadUniforms() override;
};

} // namespace Misaka
