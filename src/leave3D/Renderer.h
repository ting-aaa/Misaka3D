#pragma once

#include <glm/glm.hpp>
#include "Mesh.h"

class Renderer {
public:
    // [修复] proj 改为 const 引用
    void Render(Mesh* mesh, const glm::mat4& view, const glm::mat4& proj) {
        if (!mesh) return;

        Shader* shader = mesh->material()->GetShader();
        shader->Use();
        
        // 确保 Uniform 名称和 Shader 中一致
        shader->SetMat4("u_Model", mesh->GetWorldMatrix());
        shader->SetMat4("u_View", view);
        shader->SetMat4("u_Proj", proj);

        mesh->material()->uploadUniforms();

        mesh->geometry()->Bind();
        glDrawElements(GL_TRIANGLES, mesh->geometry()->GetIndexCount(), GL_UNSIGNED_INT, 0);
        mesh->geometry()->Unbind();
        
        shader->UnUse();
    }
};