// Camera3D.h
#pragma once
#include "Object3D.h"

class Camera3D : public Object3D {
private:
    float _fov;
    float _aspect;
    float _near;
    float _far;
    glm::mat4 _projectionMatrix;

public:
    Camera3D(float fov, float aspect, float nearPlane, float farPlane) 
        : _fov(fov), _aspect(aspect), _near(nearPlane), _far(farPlane) {
        UpdateProjection();
    }

    void SetAspect(float aspect) {
        _aspect = aspect;
        UpdateProjection();
    }

    void UpdateProjection() {
        _projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
    }

    const glm::mat4& GetProjectionMatrix() const {
        return _projectionMatrix;
    }

    // View 矩阵是摄像机世界变换的逆矩阵
    glm::mat4 GetViewMatrix() {
        return glm::inverse(GetModelMatrix());
    }
};