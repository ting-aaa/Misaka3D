#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "../resources/IResource.h"

namespace Misaka {

class Shader : public IResource {
public:
    static constexpr ResourceType TypeEnum = ResourceType::Shader;
    GLuint ID;

    Shader(const char* vCode, const char* fCode);
    ~Shader();

    // 禁用拷贝
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const;
    void UnUse() const;

    void SetMat4(const char* name, const glm::mat4& mat) const;
    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;
    void SetVec3(const char* name, const glm::vec3& vec) const;
    void SetVec4(const char* name, const glm::vec4& vec) const;
    void SetBool(const char* name, bool value) const;

    ResourceType GetResourceType() const override;
};

} // namespace Misaka
