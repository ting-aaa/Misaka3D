#pragma once

#include <memory>
#include "../entity/ModelPrefab.h"
#include "../entity/Mesh.h"
#include "../entity/ObjectContainer3D.h"

namespace Misaka {

class SceneBuilder {
public:
    static ObjectContainer3D* Instantiate(std::shared_ptr<ModelPrefab> prefab);

private:
    static ObjectContainer3D* CreateNodeRecursive(const ModelPrefab::NodeData& nodeData, std::shared_ptr<ModelPrefab> prefab);
};

} // namespace Misaka
