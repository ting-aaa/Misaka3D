#include "ColorMaterial.h"

namespace Misaka {

ColorMaterial::ColorMaterial(std::shared_ptr<Shader> shader, const glm::vec4& color)
    : MaterialBase(shader), color(color) {}

void ColorMaterial::uploadUniforms(const MaterialContext& context) {
    UploadCommonContextUniforms(context);
    _shader->SetVec4("u_Color", color);
    UploadCustomUniforms();
}

} // namespace Misaka
