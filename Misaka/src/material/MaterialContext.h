#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace Misaka {

class LightBase;

struct MaterialContext {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 cameraPosition = glm::vec3(0.0f);
    glm::vec2 resolution = glm::vec2(1.0f, 1.0f);

    float time = 0.0f;
    float deltaTime = 0.0f;
    std::uint32_t frame = 0;

    glm::vec4 mouse = glm::vec4(0.0f);
    std::array<unsigned int, 4> channels = {0, 0, 0, 0};

    const std::vector<LightBase*>* lights = nullptr;
};

} // namespace Misaka
