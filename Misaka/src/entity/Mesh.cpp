#include "Mesh.h"

namespace Misaka {

Mesh::Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<MaterialBase> material)
    : _geometry(geometry), _material(material) {}

MaterialBase* Mesh::material() const {
    return _material.get();
}

Geometry* Mesh::geometry() const {
    return _geometry.get();
}

std::shared_ptr<Geometry> Mesh::GetGeometry() const {
    return _geometry;
}

std::shared_ptr<MaterialBase> Mesh::GetMaterial() const {
    return _material;
}

void Mesh::SetGeometry(std::shared_ptr<Geometry> geometry) {
    _geometry = geometry;
}

void Mesh::SetMaterial(std::shared_ptr<MaterialBase> material) {
    _material = material;
}

glm::vec3& Mesh::GetRotation() {
    return _rotation;
}

} // namespace Misaka
