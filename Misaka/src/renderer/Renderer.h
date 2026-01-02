#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../entity/Mesh.h"
#include "../entity/light/LightBase.h"
#include "../entity/light/DirectionalLight.h"
#include "../entity/light/PointLight.h"

namespace Misaka {

class Renderer {
public:
    void Render(Mesh* mesh, const glm::mat4& view, const glm::mat4& proj, 
                const glm::vec3& cameraPos, const std::vector<LightBase*>& lights);

private:
    void uploadLights(Shader* shader, const std::vector<LightBase*>& lights);
};

} // namespace Misaka
