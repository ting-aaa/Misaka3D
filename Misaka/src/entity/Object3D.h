#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Misaka {

class Object3D {
protected:
    std::string _name;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    glm::mat4 _modelMatrix;
    bool _transformDirty;

    void UpdateMatrix();

public:
    Object3D();
    virtual ~Object3D() = default;

    void SetName(const std::string& name);
    const std::string& GetName() const;

    void SetPosition(float x, float y, float z);
    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const;

    void SetRotation(float x, float y, float z);
    void SetRotation(const glm::vec3& rot);
    glm::vec3 GetRotation() const;

    void SetScale(float x, float y, float z);
    void setScale(float s);
    void setScale(const glm::vec3& scale);
    glm::vec3 GetScale() const;

    const glm::mat4& GetModelMatrix();

    void setTransform(glm::mat4& transform);
};

} // namespace Misaka
