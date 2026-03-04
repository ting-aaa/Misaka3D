#include "MaterialBase.h"

#include <glad/glad.h>

namespace Misaka {

MaterialBase::MaterialBase(std::shared_ptr<Shader> shader) : _shader(shader) {}

void MaterialBase::UploadCommonContextUniforms(const MaterialContext& context) const {
    if (!_shader) return;

    _shader->SetMat4("u_Model", context.model);
    _shader->SetMat4("u_View", context.view);
    _shader->SetMat4("u_Proj", context.projection);
    _shader->SetVec3("u_CameraPos", context.cameraPosition);
    _shader->SetVec3("u_ViewPos", context.cameraPosition);

    _shader->SetFloat("iTime", context.time);
    _shader->SetFloat("iTimeDelta", context.deltaTime);
    _shader->SetInt("iFrame", static_cast<int>(context.frame));
    _shader->SetVec3("iResolution", glm::vec3(context.resolution, 1.0f));
    _shader->SetVec4("iMouse", context.mouse);

    for (int i = 0; i < 4; ++i) {
        const unsigned int textureID = context.channels[i];
        if (textureID != 0) {
            glActiveTexture(GL_TEXTURE4 + i);
            glBindTexture(GL_TEXTURE_2D, textureID);
        }
        const std::string channelName = "iChannel" + std::to_string(i);
        _shader->SetInt(channelName.c_str(), 4 + i);
    }
}

void MaterialBase::UploadCustomUniforms() const {
    if (!_shader) return;

    for (const auto& [name, value] : _uniforms) {
        std::visit([this, &name](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, bool>) {
                _shader->SetBool(name.c_str(), v);
            } else if constexpr (std::is_same_v<T, int>) {
                _shader->SetInt(name.c_str(), v);
            } else if constexpr (std::is_same_v<T, float>) {
                _shader->SetFloat(name.c_str(), v);
            } else if constexpr (std::is_same_v<T, glm::vec2>) {
                _shader->SetVec2(name.c_str(), v);
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                _shader->SetVec3(name.c_str(), v);
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                _shader->SetVec4(name.c_str(), v);
            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                _shader->SetMat4(name.c_str(), v);
            }
        }, value);
    }
}

void MaterialBase::SetUniformBool(const std::string& name, bool value) {
    _uniforms[name] = value;
}

void MaterialBase::SetUniformInt(const std::string& name, int value) {
    _uniforms[name] = value;
}

void MaterialBase::SetUniformFloat(const std::string& name, float value) {
    _uniforms[name] = value;
}

void MaterialBase::SetUniformVec2(const std::string& name, const glm::vec2& value) {
    _uniforms[name] = value;
}

void MaterialBase::SetUniformVec3(const std::string& name, const glm::vec3& value) {
    _uniforms[name] = value;
}

void MaterialBase::SetUniformVec4(const std::string& name, const glm::vec4& value) {
    _uniforms[name] = value;
}

void MaterialBase::SetUniformMat4(const std::string& name, const glm::mat4& value) {
    _uniforms[name] = value;
}

bool MaterialBase::RemoveUniform(const std::string& name) {
    return _uniforms.erase(name) > 0;
}

Shader* MaterialBase::GetShader() const {
    return _shader.get();
}

ResourceType MaterialBase::GetResourceType() const {
    return ResourceType::Material;
}

} // namespace Misaka
