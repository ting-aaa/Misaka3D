#pragma once

#include <memory>
#include <string>
#include "../renderer/Texture.h"

namespace Misaka {

class TextureLoader {
public:
    static std::shared_ptr<Texture> Load(std::string path);
    static std::shared_ptr<Texture> LoadFromMemory(const unsigned char* data, int dataSize, const std::string& debugName = "");
    static std::shared_ptr<Texture> LoadRawRGBA(const unsigned char* rgbaData, int width, int height);
};

} // namespace Misaka
