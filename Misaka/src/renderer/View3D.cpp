#include "View3D.h"

namespace Misaka {

View3D::View3D(Renderer* renderer, Scene3D* scene)
    : _renderer(renderer), _scene(scene) {}

void View3D::Render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos) {
    auto meshList = _scene->GetFlatMeshList();
    auto lightList = _scene->GetFlatLightList();
    for (auto mesh : meshList) {
        _renderer->Render(mesh, view, proj, cameraPos, lightList);
    }
}

} // namespace Misaka
