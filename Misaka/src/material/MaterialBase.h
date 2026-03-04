#pragma once

#include <memory>
#include <unordered_map>
#include <variant>
#include <string>
#include <glm/glm.hpp>
#include "../renderer/Shader.h"
#include "../resources/IResource.h"
#include "MaterialContext.h"

namespace Misaka {

class MaterialBase : public IResource {
protected:
    using UniformValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

    std::shared_ptr<Shader> _shader;
    std::unordered_map<std::string, UniformValue> _uniforms;

    void UploadCommonContextUniforms(const MaterialContext& context) const;
    void UploadCustomUniforms() const;

public:
    static constexpr ResourceType TypeEnum = ResourceType::Material;

    MaterialBase(std::shared_ptr<Shader> shader);
    virtual ~MaterialBase() = default;

    virtual void uploadUniforms(const MaterialContext& context) = 0;

    void SetUniformBool(const std::string& name, bool value);
    void SetUniformInt(const std::string& name, int value);
    void SetUniformFloat(const std::string& name, float value);
    void SetUniformVec2(const std::string& name, const glm::vec2& value);
    void SetUniformVec3(const std::string& name, const glm::vec3& value);
    void SetUniformVec4(const std::string& name, const glm::vec4& value);
    void SetUniformMat4(const std::string& name, const glm::mat4& value);
    bool RemoveUniform(const std::string& name);
    
    Shader* GetShader() const;

    ResourceType GetResourceType() const override;
};

} // namespace Misaka
