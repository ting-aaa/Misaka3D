#include "Scene3D.h"

namespace Misaka {

Scene3D::Scene3D() {
    _root = new ObjectContainer3D();
}

Scene3D::~Scene3D() {
    delete _root;
}

ObjectContainer3D* Scene3D::GetRoot() {
    return _root;
}

std::vector<Mesh*> Scene3D::GetFlatMeshList() {
    std::vector<Mesh*> meshList;
    auto flatChildren = _root->GetFlatChildren();
    for (auto child : flatChildren) {
        Mesh* mesh = dynamic_cast<Mesh*>(child);
        if (mesh) {
            meshList.push_back(mesh);
        }
    }
    return meshList;
}

std::vector<LightBase*> Scene3D::GetFlatLightList() {
    std::vector<LightBase*> lightList;
    auto flatChildren = _root->GetFlatChildren();
    for (auto child : flatChildren) {
        LightBase* light = dynamic_cast<LightBase*>(child);
        if (light) {
            lightList.push_back(light);
        }
    }
    return lightList;
}

} // namespace Misaka
