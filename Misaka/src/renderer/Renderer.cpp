#include "Renderer.h"
#include <glad/glad.h>
#include <string>

namespace Misaka {

void Renderer::Render(Mesh* mesh, const glm::mat4& view, const glm::mat4& proj,
                       const glm::vec3& cameraPos, const std::vector<LightBase*>& lights) {
    if (!mesh) return;

    Shader* shader = mesh->material()->GetShader();
    shader->Use();
    
    shader->SetMat4("u_Model", mesh->GetWorldMatrix());
    shader->SetMat4("u_View", view);
    shader->SetMat4("u_Proj", proj);
    shader->SetVec3("u_CameraPos", cameraPos);

    mesh->material()->uploadUniforms();
    uploadLights(shader, lights);

    mesh->geometry()->Bind();
    glDrawElements(GL_TRIANGLES, mesh->geometry()->GetIndexCount(), GL_UNSIGNED_INT, 0);
    mesh->geometry()->Unbind();
    
    shader->UnUse();
}

void Renderer::uploadLights(Shader* shader, const std::vector<LightBase*>& lights) {
    int dirLightCount = 0;
    int pointLightCount = 0;

    for (auto light : lights) {
        if (light->GetLightType() == LightType::Directional) {
            if (dirLightCount >= 2) continue;

            DirectionalLight* dirLight = static_cast<DirectionalLight*>(light);
            std::string base = "u_DirLights[" + std::to_string(dirLightCount) + "]";
            
            shader->SetVec3((base + ".direction").c_str(), dirLight->GetDirection());
            shader->SetVec3((base + ".color").c_str(), dirLight->color);
            shader->SetFloat((base + ".intensity").c_str(), dirLight->intensity);
            
            dirLightCount++;
        }
        else if (light->GetLightType() == LightType::Point) {
            if (pointLightCount >= 4) continue;

            PointLight* pointLight = static_cast<PointLight*>(light);
            std::string base = "u_PointLights[" + std::to_string(pointLightCount) + "]";
            
            shader->SetVec3((base + ".position").c_str(), pointLight->GetPosition());
            shader->SetVec3((base + ".color").c_str(), pointLight->color);
            shader->SetFloat((base + ".intensity").c_str(), pointLight->intensity);
            shader->SetFloat((base + ".constant").c_str(), pointLight->constant);
            shader->SetFloat((base + ".linear").c_str(), pointLight->linear);
            shader->SetFloat((base + ".quadratic").c_str(), pointLight->quadratic);
            
            pointLightCount++;
        }
    }

    shader->SetInt("u_DirLightCount", dirLightCount);
    shader->SetInt("u_PointLightCount", pointLightCount);
}

} // namespace Misaka
