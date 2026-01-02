#include "TextureLoader.h"
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

namespace Misaka {

std::shared_ptr<Texture> TextureLoader::Load(std::string path) {
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
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

        stbi_image_free(data);
        texture->id = textureID;
        texture->width = width;
        texture->height = height;
        texture->channels = nrComponents;
        return texture;
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return nullptr;
    }
}

} // namespace Misaka
