#include "Shader.h"

namespace Misaka {

Shader::Shader(const char* vCode, const char* fCode) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, nullptr);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, nullptr);
    glCompileShader(fragment);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

void Shader::Use() const {
    glUseProgram(ID);
}

void Shader::UnUse() const {
    glUseProgram(0);
}

void Shader::SetMat4(const char* name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetVec3(const char* name, const glm::vec3& vec) const {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &vec[0]);
}

void Shader::SetVec4(const char* name, const glm::vec4& vec) const {
    glUniform4fv(glGetUniformLocation(ID, name), 1, &vec[0]);
}

void Shader::SetBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

ResourceType Shader::GetResourceType() const {
    return ResourceType::Shader;
}

} // namespace Misaka
