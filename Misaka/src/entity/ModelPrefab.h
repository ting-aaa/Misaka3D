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

    ResourceType GetResourceType() const override {
        return ResourceType::ModelPrefab;
    }
};

} // namespace Misaka
