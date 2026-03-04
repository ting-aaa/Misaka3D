#include "TextureLoader.h"
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

namespace Misaka {

static std::shared_ptr<Texture> CreateTextureFromPixels(const unsigned char* data, int width, int height, int nrComponents) {
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    unsigned int textureID;
    glGenTextures(1, &textureID);

    GLenum format = GL_RGB;
    if (nrComponents == 1) format = GL_RED;
    else if (nrComponents == 3) format = GL_RGB;
    else if (nrComponents == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture->id = textureID;
    texture->width = width;
    texture->height = height;
    texture->channels = nrComponents;
    return texture;
}

std::shared_ptr<Texture> TextureLoader::Load(std::string path) {
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        auto texture = CreateTextureFromPixels(data, width, height, nrComponents);
        stbi_image_free(data);
        return texture;
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return nullptr;
    }
}

std::shared_ptr<Texture> TextureLoader::LoadFromMemory(const unsigned char* data, int dataSize, const std::string& debugName) {
    if (!data || dataSize <= 0) {
        std::cout << "Texture memory data is invalid: " << debugName << std::endl;
        return nullptr;
    }

    int width = 0;
    int height = 0;
    int nrComponents = 0;
    unsigned char* pixels = stbi_load_from_memory(data, dataSize, &width, &height, &nrComponents, 0);
    if (!pixels) {
        std::cout << "Texture failed to load from memory: " << debugName << std::endl;
        return nullptr;
    }

    auto texture = CreateTextureFromPixels(pixels, width, height, nrComponents);
    stbi_image_free(pixels);
    return texture;
}

std::shared_ptr<Texture> TextureLoader::LoadRawRGBA(const unsigned char* rgbaData, int width, int height) {
    if (!rgbaData || width <= 0 || height <= 0) {
        std::cout << "Raw RGBA texture data is invalid." << std::endl;
        return nullptr;
    }
    return CreateTextureFromPixels(rgbaData, width, height, 4);
}

} // namespace Misaka
