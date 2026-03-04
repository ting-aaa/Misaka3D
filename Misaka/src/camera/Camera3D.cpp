#include "Camera3D.h"

namespace Misaka {

Camera3D::Camera3D(float fov, float aspect, float nearPlane, float farPlane)
    : _fov(fov), _aspect(aspect), _near(nearPlane), _far(farPlane) {
    UpdateProjection();
}

void Camera3D::SetFov(float fov) {
    _fov = fov;
    UpdateProjection();
}

void Camera3D::SetAspect(float aspect) {
    _aspect = aspect;
    UpdateProjection();
}

void Camera3D::SetNearFar(float nearPlane, float farPlane) {
    _near = nearPlane;
    _far = farPlane;
    UpdateProjection();
}

void Camera3D::UpdateProjection() {
    _projectionMatrix = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
}

const glm::mat4& Camera3D::GetProjectionMatrix() const {
    return _projectionMatrix;
}

float Camera3D::GetFov() const {
    return _fov;
}

float Camera3D::GetAspect() const {
    return _aspect;
}

float Camera3D::GetNearPlane() const {
    return _near;
}

float Camera3D::GetFarPlane() const {
    return _far;
}

glm::mat4 Camera3D::GetViewMatrix() {
    return glm::inverse(GetModelMatrix());
}

glm::mat4 Camera3D::GetViewProjectionMatrix() {
    return GetProjectionMatrix() * GetViewMatrix();
}

} // namespace Misaka
