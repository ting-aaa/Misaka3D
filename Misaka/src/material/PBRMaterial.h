#pragma once

#include "MaterialBase.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Misaka {

class PBRMaterial : public MaterialBase {
public:
    unsigned int albedoMap = 0;
    unsigned int normalMap = 0;
    unsigned int metallicMap = 0;
    unsigned int roughnessMap = 0;
    unsigned int aoMap = 0;
    unsigned int emissiveMap = 0;

    glm::vec4 albedoColor = glm::vec4(1.0f);
    glm::vec3 emissiveColor = glm::vec3(0.0f);

    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    explicit PBRMaterial(std::shared_ptr<Shader> shader);

    void uploadUniforms(const MaterialContext& context) override;
};

} // namespace Misaka
