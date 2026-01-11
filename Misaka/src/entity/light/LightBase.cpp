#include "LightBase.h"

namespace Misaka {

LightBase::LightBase(LightType type)
    : _lightType(type), color(glm::vec3(1.0, 1.0, 1.0)), intensity(1.0f), enabled(true) {}

LightType LightBase::GetLightType() const {
    return _lightType;
}

} // namespace Misaka
