#include "CommonMaterial.h"

namespace Misaka {

CommonMaterial::CommonMaterial(std::shared_ptr<Shader> shader) : MaterialBase(shader) {}

void CommonMaterial::uploadUniforms() {
    Shader* s = _shader.get();

    // --- 1. Albedo ---
    if (albedoMap != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        s->SetInt("u_AlbedoMap", 0);
        s->SetBool("u_HasAlbedoMap", true);
    } else {
        s->SetBool("u_HasAlbedoMap", false);
        s->SetVec4("u_AlbedoColor", color);
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
        s->SetFloat("u_RoughnessVal", roughness);
    }

    // --- 4. Metallic ---
    if (metallicMap != 0) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, metallicMap);
        s->SetInt("u_MetallicMap", 3);
        s->SetBool("u_HasMetallicMap", true);
    } else {
        s->SetBool("u_HasMetallicMap", false);
        s->SetFloat("u_MetallicVal", metallic);
    }

    // --- 5. 光照 ---
    s->SetVec3("u_LightDir", lightDirection);
}

} // namespace Misaka
