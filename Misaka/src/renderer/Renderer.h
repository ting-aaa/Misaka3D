#pragma once

#include <vector>
#include <chrono>
#include <cstdint>
#include <glm/glm.hpp>
#include "../entity/Mesh.h"
#include "../entity/light/LightBase.h"
#include "../entity/light/DirectionalLight.h"
#include "../entity/light/PointLight.h"
#include "../material/MaterialContext.h"

namespace Misaka {

class Renderer {
public:
    Renderer();

    void Render(Mesh* mesh, const glm::mat4& view, const glm::mat4& proj, 
                const glm::vec3& cameraPos, const std::vector<LightBase*>& lights);

private:
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _lastFrameTime;
    std::uint32_t _frameCounter = 0;

    void uploadLights(Shader* shader, const std::vector<LightBase*>& lights);
};

} // namespace Misaka
