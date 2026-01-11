#include "Texture.h"

namespace Misaka {

Texture::~Texture() {
    if (id) {
        glDeleteTextures(1, &id);
    }
}

void Texture::Bind(int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

ResourceType Texture::GetResourceType() const {
    return ResourceType::Texture;
}

} // namespace Misaka
