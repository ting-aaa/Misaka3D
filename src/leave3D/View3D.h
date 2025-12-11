#pragma once

#include "Renderer.h"
#include "Scene3D.h"

class View3D {
public:
    View3D(Renderer* renderer, Scene3D* scene) : _renderer(renderer), _scene(scene) {}

    // [修复] proj 改为 const 引用
    void Render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos) {
        auto meshList = _scene->GetFlatMeshList();
        auto lightList = _scene->GetFlatLightList();
        for(auto mesh : meshList) {
            _renderer->Render(mesh, view, proj, cameraPos, lightList);
        }
    }
private:
    Renderer* _renderer;
    Scene3D* _scene;
};