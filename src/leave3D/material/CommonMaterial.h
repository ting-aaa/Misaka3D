#pragma once

#include "MaterialBase.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class CommonMaterial : public MaterialBase {
public:
    // ==========================================
    // 材质属性 (Public 方便直接修改)
    // ==========================================
    
    // 1. 基础颜色 (Albedo)
    unsigned int albedoMap = 0;      // 纹理ID，0表示空
    glm::vec4 color = glm::vec4(1.0f); // 缺省颜色 (白色)

    // 2. 法线 (Normal)
    unsigned int normalMap = 0; 
    // 法线没有"缺省值"，缺省就是几何体法线

    // 3. 粗糙度 (Roughness)
    unsigned int roughnessMap = 0;
    float roughness = 0.5f;          // 缺省粗糙度 (0.0-1.0)

    // 4. 金属度 (Metallic)
    unsigned int metallicMap = 0;
    float metallic = 0.0f;           // 缺省金属度 (0.0-1.0)

    // 光照方向 (实际项目中通常由 Scene/Light 组件传入，这里为了演示方便放在材质里)
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.5f, 1.0f, 0.5f));

    CommonMaterial(std::shared_ptr<Shader> shader) : MaterialBase(shader) {}

    // ==========================================
    // 核心逻辑: 上传 Uniform
    // ==========================================
    void uploadUniforms() override {
        Shader* s = _shader.get();

        // --- 1. Albedo ---
        if (albedoMap != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, albedoMap);
            s->SetInt("u_AlbedoMap", 0);
            s->SetBool("u_HasAlbedoMap", true);
        } else {
            s->SetBool("u_HasAlbedoMap", false);
            s->SetVec4("u_AlbedoColor", color); // 上传备用颜色
        }

        // --- 2. Normal Map ---
        if (normalMap != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalMap);
            s->SetInt("u_NormalMap", 1);
            s->SetBool("u_HasNormalMap", true);
        } else {
            s->SetBool("u_HasNormalMap", false);
        }

        // --- 3. Roughness ---
        if (roughnessMap != 0) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, roughnessMap);
            s->SetInt("u_RoughnessMap", 2);
            s->SetBool("u_HasRoughnessMap", true);
        } else {
            s->SetBool("u_HasRoughnessMap", false);
            s->SetFloat("u_RoughnessVal", roughness); // 上传备用浮点数
        }

        // --- 4. Metallic ---
        if (metallicMap != 0) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, metallicMap);
            s->SetInt("u_MetallicMap", 3);
            s->SetBool("u_HasMetallicMap", true);
        } else {
            s->SetBool("u_HasMetallicMap", false);
            s->SetFloat("u_MetallicVal", metallic); // 上传备用浮点数
        }

        // --- 5. 光照与其他 ---
        s->SetVec3("u_LightDir", lightDirection);
        // 注意：u_ViewPos 由 Renderer 传入，材质不需要管
    }
};