#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Object3D {
protected:
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    glm::mat4 _modelMatrix;
    bool _transformDirty;   //脏标记

    void UpdateMatrix() {
        if (!_transformDirty) return;
        _modelMatrix = glm::mat4(1.0f);
        _modelMatrix = glm::translate(_modelMatrix, _position);
        _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.x), glm::vec3(1,0,0));
        _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.y), glm::vec3(0,1,0));
        _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.z), glm::vec3(0,0,1));
        _modelMatrix = glm::scale(_modelMatrix, _scale);
        _transformDirty = false;
    }
public:
    Object3D() : _position(0), _rotation(0), _scale(1), _transformDirty(true) {}
    virtual ~Object3D() = default;

    // Getters & Setters (设置时标记 dirty = true)
    void SetPosition(float x, float y, float z) { _position = {x,y,z}; _transformDirty = true; }
    void SetRotation(float x, float y, float z) { _rotation = {x,y,z}; _transformDirty = true; }
    void SetScale(float x, float y, float z)    { _scale = {x,y,z};    _transformDirty = true; }
    glm::vec3& GetPosition() { return _position; }
    glm::vec3& GetRotation() { return _rotation; }
    glm::vec3& GetScale()    { return _scale;    }
    
    // 获取最终矩阵
    const glm::mat4& GetModelMatrix() {
        UpdateMatrix();
        return _modelMatrix;
    }
};