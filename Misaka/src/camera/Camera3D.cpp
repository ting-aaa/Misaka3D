#include "Camera3D.h"

namespace Misaka {

Camera3D::Camera3D(float fov, float aspect, float nearPlane, float farPlane)
    : _fov(fov), _aspect(aspect), _near(nearPlane), _far(farPlane) {
    UpdateProjection();
}

void Camera3D::SetAspect(float aspect) {
    _aspect = aspect;
    UpdateProjection();
}

void Camera3D::UpdateProjection() {
    _projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
}

const glm::mat4& Camera3D::GetProjectionMatrix() const {
    return _projectionMatrix;
}

glm::mat4 Camera3D::GetViewMatrix() {
    return glm::inverse(GetModelMatrix());
}

} // namespace Misaka
