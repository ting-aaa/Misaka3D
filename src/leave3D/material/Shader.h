#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string> // 建议加上string头文件

#include "../resources/IResource.h"

class Shader : public IResource {
public:
    static constexpr ResourceType TypeEnum = ResourceType::Shader;
    GLuint ID;

    Shader(const char* vCode, const char* fCode) {
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vCode, nullptr);
        glCompileShader(vertex);
        // 建议：此处实际开发应加入编译错误检查

        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fCode, nullptr);
        glCompileShader(fragment);
        // 建议：此处实际开发应加入编译错误检查

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        // 建议：此处实际开发应加入链接错误检查

        // Shader Program 链接完成后，单独的 shader 对象就可以删除了
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // [修复] 添加析构函数释放 Program
    ~Shader() {
        if (ID != 0) {
            glDeleteProgram(ID);
            ID = 0;
        }
    }

    // 禁用拷贝构造，防止 ID 被重复 delete (RAII)
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const { glUseProgram(ID); } // 加上 const
    void UnUse() const { glUseProgram(0); }

    void SetMat4(const char* name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
    }

    void SetInt(const char* name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void SetFloat(const char* name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void SetVec3(const char* name, const glm::vec3& vec) const {
        glUniform3fv(glGetUniformLocation(ID, name), 1, &vec[0]);
    }

    void SetVec4(const char* name, const glm::vec4& vec) const {
        glUniform4fv(glGetUniformLocation(ID, name), 1, &vec[0]);
    }

    void SetBool(const char* name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name), (int)value);
    }

    ResourceType GetResourceType() const override { return ResourceType::Shader; }
};