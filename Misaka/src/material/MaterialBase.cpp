#include "MaterialBase.h"

namespace Misaka {

MaterialBase::MaterialBase(std::shared_ptr<Shader> shader) : _shader(shader) {}

Shader* MaterialBase::GetShader() const {
    return _shader.get();
}

ResourceType MaterialBase::GetResourceType() const {
    return ResourceType::Material;
}

} // namespace Misaka
