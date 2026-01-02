#include "Object3D.h"

namespace Misaka {

Object3D::Object3D() 
    : _position(0), _rotation(0), _scale(1), _transformDirty(true) {}

void Object3D::UpdateMatrix() {
    if (!_transformDirty) return;
    _modelMatrix = glm::mat4(1.0f);
    _modelMatrix = glm::translate(_modelMatrix, _position);
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.x), glm::vec3(1, 0, 0));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.y), glm::vec3(0, 1, 0));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(_rotation.z), glm::vec3(0, 0, 1));
    _modelMatrix = glm::scale(_modelMatrix, _scale);
    _transformDirty = false;
}

void Object3D::SetName(const std::string& name) {
    _name = name;
}

const std::string& Object3D::GetName() const {
    return _name;
}

void Object3D::SetPosition(float x, float y, float z) {
    _position = {x, y, z};
    _transformDirty = true;
}

void Object3D::SetPosition(const glm::vec3& pos) {
    _position = pos;
    _transformDirty = true;
}

glm::vec3 Object3D::GetPosition() const {
    return _position;
}

void Object3D::SetRotation(float x, float y, float z) {
    _rotation = {x, y, z};
    _transformDirty = true;
}

void Object3D::SetRotation(const glm::vec3& rot) {
    _rotation = rot;
    _transformDirty = true;
}

glm::vec3 Object3D::GetRotation() const {
    return _rotation;
}

void Object3D::SetScale(float x, float y, float z) {
    _scale = {x, y, z};
    _transformDirty = true;
}

void Object3D::setScale(float s) {
    _scale = {s, s, s};
    _transformDirty = true;
}

void Object3D::setScale(const glm::vec3& scale) {
    _scale = scale;
    _transformDirty = true;
}

glm::vec3 Object3D::GetScale() const {
    return _scale;
}

const glm::mat4& Object3D::GetModelMatrix() {
    UpdateMatrix();
    return _modelMatrix;
}

void Object3D::setTransform(glm::mat4& transform) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    if (glm::decompose(transform, scale, rotation, translation, skew, perspective)) {
        _position = translation;
        _scale = scale;
        
        glm::vec3 eulerRad = glm::eulerAngles(rotation);
        _rotation = glm::degrees(eulerRad);
        
        _transformDirty = true;
    }
}

} // namespace Misaka
