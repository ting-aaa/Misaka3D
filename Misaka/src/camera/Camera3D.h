#pragma once

#include "../entity/Object3D.h"

namespace Misaka {

class Camera3D : public Object3D {
private:
    float _fov;
    float _aspect;
    float _near;
    float _far;
    glm::mat4 _projectionMatrix;

public:
    Camera3D(float fov, float aspect, float nearPlane, float farPlane);

    void SetAspect(float aspect);
    void UpdateProjection();

    const glm::mat4& GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix();
};

} // namespace Misaka
