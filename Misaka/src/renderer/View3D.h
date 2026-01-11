#pragma once

#include "Renderer.h"
#include "../scene/Scene3D.h"

namespace Misaka {

class View3D {
public:
    View3D(Renderer* renderer, Scene3D* scene);

    void Render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos);

private:
    Renderer* _renderer;
    Scene3D* _scene;
};

} // namespace Misaka
