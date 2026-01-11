#pragma once

#include <vector>
#include <type_traits>
#include "../entity/Object3D.h"
#include "../entity/ObjectContainer3D.h"
#include "../entity/Mesh.h"
#include "../entity/light/LightBase.h"

namespace Misaka {

class Scene3D {
protected:
    ObjectContainer3D* _root;

public:
    Scene3D();
    ~Scene3D();

    ObjectContainer3D* GetRoot();
    std::vector<Mesh*> GetFlatMeshList();
    std::vector<LightBase*> GetFlatLightList();
};

} // namespace Misaka
