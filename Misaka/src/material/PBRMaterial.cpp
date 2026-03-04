#include "PBRMaterial.h"

namespace Misaka {

PBRMaterial::PBRMaterial(std::shared_ptr<Shader> shader)
    : MaterialBase(shader) {}

void PBRMaterial::uploadUniforms(const MaterialContext& context) {
    UploadCommonContextUniforms(context);

    if (albedoMap != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap);
        _shader->SetInt("u_AlbedoMap", 0);
        _shader->SetBool("u_HasAlbedoMap", true);
    } else {
        _shader->SetBool("u_HasAlbedoMap", false);
        _shader->SetVec4("u_AlbedoColor", albedoColor);
    }

    if (normalMap != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        _shader->SetInt("u_NormalMap", 1);
        _shader->SetBool("u_HasNormalMap", true);
    } else {
        _shader->SetBool("u_HasNormalMap", false);
    }

    if (roughnessMap != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughnessMap);
        _shader->SetInt("u_RoughnessMap", 2);
        _shader->SetBool("u_HasRoughnessMap", true);
    } else {
        _shader->SetBool("u_HasRoughnessMap", false);
        _shader->SetFloat("u_RoughnessVal", roughness);
    }

    if (metallicMap != 0) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, metallicMap);
        _shader->SetInt("u_MetallicMap", 3);
        _shader->SetBool("u_HasMetallicMap", true);
    } else {
        _shader->SetBool("u_HasMetallicMap", false);
        _shader->SetFloat("u_MetallicVal", metallic);
    }

    if (aoMap != 0) {
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, aoMap);
        _shader->SetInt("u_AoMap", 8);
        _shader->SetBool("u_HasAoMap", true);
    } else {
        _shader->SetBool("u_HasAoMap", false);
        _shader->SetFloat("u_AoVal", ao);
    }

    if (emissiveMap != 0) {
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, emissiveMap);
        _shader->SetInt("u_EmissiveMap", 9);
        _shader->SetBool("u_HasEmissiveMap", true);
    } else {
        _shader->SetBool("u_HasEmissiveMap", false);
        _shader->SetVec3("u_EmissiveColor", emissiveColor);
    }

    UploadCustomUniforms();
}

} // namespace Misaka
