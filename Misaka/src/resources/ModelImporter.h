#pragma once

#include <memory>
#include <string>
#include "../entity/ModelPrefab.h"

namespace Misaka {

class ModelImporter {
public:
    static std::shared_ptr<ModelPrefab> Load(const std::string& path);
};

} // namespace Misaka
