#pragma once

#include <memory>
#include <string>
#include "../renderer/Texture.h"

namespace Misaka {

class TextureLoader {
public:
    static std::shared_ptr<Texture> Load(std::string path);
};

} // namespace Misaka
