#pragma once

#include <string>

#include "Scene3D.h"
#include "../camera/Camera3D.h"

namespace Misaka {

struct SceneLoadResult {
    bool success = false;
    std::string error;
    Camera3D* camera = nullptr;
};

class SceneConfigLoader {
public:
    static SceneLoadResult LoadFromFile(Scene3D* scene, const std::string& configPath);
};

} // namespace Misaka
