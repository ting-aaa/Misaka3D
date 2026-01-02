#include "SceneBuilder.h"

namespace Misaka {

ObjectContainer3D* SceneBuilder::Instantiate(std::shared_ptr<ModelPrefab> prefab) {
    return CreateNodeRecursive(prefab->rootNode, prefab);
}

ObjectContainer3D* SceneBuilder::CreateNodeRecursive(const ModelPrefab::NodeData& nodeData, std::shared_ptr<ModelPrefab> prefab) {
    ObjectContainer3D* obj = nullptr;
    
    if (nodeData.meshIndices.empty()) {
        obj = new ObjectContainer3D();
    } else if (nodeData.meshIndices.size() == 1) {
        int meshIdx = nodeData.meshIndices[0];
        auto& meshEntry = prefab->meshes[meshIdx];
        obj = new Mesh(meshEntry.geometry, meshEntry.material);
    } else {
        obj = new ObjectContainer3D();
        for (int meshIdx : nodeData.meshIndices) {
            auto& meshEntry = prefab->meshes[meshIdx];
            Mesh* mesh = new Mesh(meshEntry.geometry, meshEntry.material);
            mesh->SetName(meshEntry.name);
            glm::mat4 transform = nodeData.localTransform;
            mesh->setTransform(transform);
            obj->AddChild(mesh);
        }
    }

    obj->SetName(nodeData.name);
    glm::mat4 transform = nodeData.localTransform;
    obj->setTransform(transform);

    for (const auto& childData : nodeData.children) {
        ObjectContainer3D* childObj = CreateNodeRecursive(childData, prefab);
        obj->AddChild(childObj);
    }
    
    return obj;
}

} // namespace Misaka
