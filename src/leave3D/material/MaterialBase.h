#pragma once
#include "Shader.h"

class MaterialBase {
protected:
    Shader* _shader;
public:
    MaterialBase(Shader* shader) : _shader(shader) {}
    
    // [修复] 必须有虚析构函数
    virtual ~MaterialBase() = default;

    virtual void uploadUniforms() = 0;
    
    Shader* GetShader() const { return _shader; }
};

class ColorMaterial : public MaterialBase {
public:
    glm::vec4 color;
    ColorMaterial(Shader* shader, const glm::vec4& color) : MaterialBase(shader), color(color) {}

    void uploadUniforms() override {
        _shader->SetVec4("u_Color", color);
    }
};