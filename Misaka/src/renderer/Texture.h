#pragma once

#include <glad/glad.h>

#include "../resources/IResource.h"

namespace Misaka {

class Texture : public IResource {
public:
    static constexpr ResourceType TypeEnum = ResourceType::Texture;

    GLuint id = 0;
    int width = 0, height = 0, channels = 0;

    Texture() = default;
    virtual ~Texture();

    // 禁用拷贝
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void Bind(int slot = 0) const;

    ResourceType GetResourceType() const override;
};

} // namespace Misaka
