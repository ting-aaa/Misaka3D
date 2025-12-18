#pragma once
#include <glad/glad.h>

#include "resources/IResource.h"

class Texture : public IResource {
public:
    static constexpr ResourceType TypeEnum = ResourceType::Texture;

    GLuint id = 0;
    int width = 0, height = 0, channels = 0;

    Texture() = default;
    virtual ~Texture() {
        if ( id ) {
            glDeleteTextures(1, &id);
        }
    }

    void Bind(int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    ResourceType GetResourceType() const override { return ResourceType::Texture; }
};