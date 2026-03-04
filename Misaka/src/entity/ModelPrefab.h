#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "../renderer/Geometry.h"
#include "../material/MaterialBase.h"
#include "../resources/IResource.h"

namespace Misaka {

struct ModelPrefab : public IResource {
    static constexpr ResourceType TypeEnum = ResourceType::ModelPrefab;
    
    struct MeshEntry {
        std::string name;
        std::shared_ptr<Geometry> geometry;
        std::shared_ptr<MaterialBase> material;
    };

    struct NodeData {
        std::string name;
        glm::mat4 localTransform;
        std::vector<int> meshIndices;
        std::vector<NodeData> children;
    };

    std::vector<MeshEntry> meshes;
    NodeData rootNode;

    size_t GetMeshCount() const {
        return meshes.size();
    }

    MeshEntry* GetMeshEntry(size_t index) {
        if (index >= meshes.size()) return nullptr;
        return &meshes[index];
    }

    const MeshEntry* GetMeshEntry(size_t index) const {
        if (index >= meshes.size()) return nullptr;
        return &meshes[index];
    }

    std::shared_ptr<MaterialBase> GetMaterial(size_t meshIndex) const {
        if (meshIndex >= meshes.size()) return nullptr;
        return meshes[meshIndex].material;
    }

    bool SetMaterial(size_t meshIndex, std::shared_ptr<MaterialBase> material) {
        if (meshIndex >= meshes.size() || !material) return false;
        meshes[meshIndex].material = material;
        return true;
    }

    bool SetMaterialByMeshName(const std::string& meshName, std::shared_ptr<MaterialBase> material) {
        if (!material) return false;
        bool updated = false;
        for (auto& mesh : meshes) {
            if (mesh.name == meshName) {
                mesh.material = material;
                updated = true;
            }
        }
        return updated;
    }

    ResourceType GetResourceType() const override {
        return ResourceType::ModelPrefab;
    }
};

} // namespace Misaka
