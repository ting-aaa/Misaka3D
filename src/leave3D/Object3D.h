#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

class Object3D {
protected:
    std::string _name;
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

    void SetName(const std::string& name) { _name = name; }
    const std::string& GetName() const { return _name; }

    // Getters & Setters (设置时标记 dirty = true)
    void SetPosition(float x, float y, float z) { _position = {x,y,z}; _transformDirty = true; }
    void SetPosition(const glm::vec3& pos) {  _position = pos; _transformDirty = true; }
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(float x, float y, float z) { _rotation = {x,y,z}; _transformDirty = true; }
    void SetRotation(const glm::vec3& rot) {  _rotation = rot; _transformDirty = true; }
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(float x, float y, float z)    { _scale = {x,y,z};    _transformDirty = true; }
    void setScale(float s) {  _scale = {s,s,s};    _transformDirty = true; }
    void setScale(const glm::vec3& scale) {  _scale = scale;    _transformDirty = true; }
    glm::vec3 GetScale() const { return _scale; }
    
    // 获取最终矩阵
    const glm::mat4& GetModelMatrix() {
        UpdateMatrix();
        return _modelMatrix;
    }

    void setTransform(glm::mat4& transform) {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        // 使用 GLM 分解矩阵
        if (glm::decompose(transform, scale, rotation, translation, skew, perspective)) {
            _position = translation;
            _scale = scale;
            
            // 四元数转欧拉角 (GLM 返回的是弧度)
            glm::vec3 eulerRad = glm::eulerAngles(rotation);
            _rotation = glm::degrees(eulerRad);
            
            _transformDirty = true; // 标记脏，确保下一次 GetModelMatrix 重新计算
        }
    }
};