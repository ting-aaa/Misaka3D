#pragma once
#include <vector>
#include <glad/glad.h>
#include <cstddef> // offsetof

#include "resources/IResource.h"

struct Vertex {
    float position[3];
    float normal[3];
    float texCoords[2];
    float tangent[3];
};

class Geometry : public IResource{
public:
    static constexpr ResourceType TypeEnum = ResourceType::Geometry;

    Geometry() : _vao(0), _vbo(0), _ebo(0), _indexCount(0) {}
    Geometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
        UploadGPU(vertices, indices);
    }

    void UploadGPU(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
        _indexCount = indices.size();
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);

        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // attribute, 使用Vertex结构体自动布局
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    ~Geometry() {
        if (_vao) glDeleteVertexArrays(1, &_vao);
        if (_vbo) glDeleteBuffers(1, &_vbo);
        if (_ebo) glDeleteBuffers(1, &_ebo);
    }

    // 禁用拷贝，防止 double free
    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;

    void Bind() const { glBindVertexArray(_vao); }
    void Unbind() const { glBindVertexArray(0); }

    int GetIndexCount() const { return _indexCount; }

    ResourceType GetResourceType() const override { return ResourceType::Geometry; }

    static std::vector<float> ComputeTangents(const std::vector<float>& oldVertices, const std::vector<unsigned int>& indices) {
        // 1. 初始化新容器
        int numVertices = oldVertices.size() / 8; // 假设旧格式是 8 float
        std::vector<float> newVertices;
        newVertices.resize(numVertices * 11); // 新格式 11 float

        // 临时切线累加器
        std::vector<glm::vec3> tempTangents(numVertices, glm::vec3(0.0f));

        // 2. 遍历三角形计算切线
        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i+1];
            unsigned int i2 = indices[i+2];

            // 获取位置
            glm::vec3 pos0 = GetVec3(oldVertices, i0, 0);
            glm::vec3 pos1 = GetVec3(oldVertices, i1, 0);
            glm::vec3 pos2 = GetVec3(oldVertices, i2, 0);

            // 获取UV
            glm::vec2 uv0 = GetVec2(oldVertices, i0, 6);
            glm::vec2 uv1 = GetVec2(oldVertices, i1, 6);
            glm::vec2 uv2 = GetVec2(oldVertices, i2, 6);

            // 计算边和Delta UV
            glm::vec3 edge1 = pos1 - pos0;
            glm::vec3 edge2 = pos2 - pos0;
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            // 切线公式
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            // 累加到顶点 (用于平滑)
            tempTangents[i0] += tangent;
            tempTangents[i1] += tangent;
            tempTangents[i2] += tangent;
        }

        // 3. 组装数据并正交化 (Gram-Schmidt)
        for (int i = 0; i < numVertices; ++i) {
            glm::vec3 n = GetVec3(oldVertices, i, 3); // 原法线
            glm::vec3 t = tempTangents[i];
            
            // Gram-Schmidt 正交化: t' = normalize(t - dot(t, n) * n)
            glm::vec3 tangent = glm::normalize(t - n * glm::dot(n, t));

            // 填入新数组
            // Pos
            newVertices[i*11 + 0] = oldVertices[i*8 + 0];
            newVertices[i*11 + 1] = oldVertices[i*8 + 1];
            newVertices[i*11 + 2] = oldVertices[i*8 + 2];
            // Normal
            newVertices[i*11 + 3] = oldVertices[i*8 + 3];
            newVertices[i*11 + 4] = oldVertices[i*8 + 4];
            newVertices[i*11 + 5] = oldVertices[i*8 + 5];
            // UV
            newVertices[i*11 + 6] = oldVertices[i*8 + 6];
            newVertices[i*11 + 7] = oldVertices[i*8 + 7];
            // Tangent
            newVertices[i*11 + 8] = tangent.x;
            newVertices[i*11 + 9] = tangent.y;
            newVertices[i*11 + 10] = tangent.z;
        }

        return newVertices;
    }
private:
    GLuint _vao = 0, _vbo = 0, _ebo = 0;
    int _indexCount = 0;

    static glm::vec3 GetVec3(const std::vector<float>& v, int index, int offset) {
        return glm::vec3(v[index * 8 + offset], v[index * 8 + offset + 1], v[index * 8 + offset + 2]);
    }
    static glm::vec2 GetVec2(const std::vector<float>& v, int index, int offset) {
        return glm::vec2(v[index * 8 + offset], v[index * 8 + offset + 1]);
    }
};