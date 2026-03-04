#pragma once

#include "MaterialBase.h"
#include <glm/glm.hpp>

namespace Misaka {

class ColorMaterial : public MaterialBase {
public:
    glm::vec4 color;
    
    ColorMaterial(std::shared_ptr<Shader> shader, const glm::vec4& color);

    void uploadUniforms(const MaterialContext& context) override;
};

} // namespace Misaka
