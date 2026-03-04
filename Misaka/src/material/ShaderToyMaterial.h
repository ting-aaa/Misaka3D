#pragma once

#include <array>

#include "MaterialBase.h"

namespace Misaka {

class ShaderToyMaterial : public MaterialBase {
public:
    std::array<unsigned int, 4> channels = {0, 0, 0, 0};

    explicit ShaderToyMaterial(std::shared_ptr<Shader> shader);

    void uploadUniforms(const MaterialContext& context) override;
};

} // namespace Misaka
