#pragma once
#include "Shader.h"

#include "../resources/IResource.h"

class MaterialBase : public IResource{
protected:
    std::shared_ptr<Shader>_shader;
public:
    static constexpr ResourceType TypeEnum = ResourceType::Material;

    MaterialBase(std::shared_ptr<Shader> shader) : _shader(shader) {}
    
    // [修复] 必须有虚析构函数
    virtual ~MaterialBase() = default;

    virtual void uploadUniforms() = 0;
    
    Shader* GetShader() const { return _shader.get(); }

    ResourceType GetResourceType() const override { return ResourceType::Material; }
};

class ColorMaterial : public MaterialBase {
public:
    glm::vec4 color;
    ColorMaterial(std::shared_ptr<Shader> shader, const glm::vec4& color) : MaterialBase(shader), color(color) {}

    void uploadUniforms() override {
        _shader->SetVec4("u_Color", color);
    }
};