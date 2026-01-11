#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstddef>

#include "../resources/IResource.h"

namespace Misaka {

struct Vertex {
    float position[3];
    float normal[3];
    float texCoords[2];
    float tangent[3];
};

class Geometry : public IResource {
public:
    static constexpr ResourceType TypeEnum = ResourceType::Geometry;

    Geometry();
    Geometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~Geometry();

    // 禁用拷贝
    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;

    void UploadGPU(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void Bind() const;
    void Unbind() const;
    int GetIndexCount() const;

    ResourceType GetResourceType() const override;

    static std::vector<float> ComputeTangents(const std::vector<float>& oldVertices, const std::vector<unsigned int>& indices);

private:
    GLuint _vao = 0, _vbo = 0, _ebo = 0;
    int _indexCount = 0;

    static glm::vec3 GetVec3(const std::vector<float>& v, int index, int offset);
    static glm::vec2 GetVec2(const std::vector<float>& v, int index, int offset);
};

} // namespace Misaka
