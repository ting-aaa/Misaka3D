#pragma once
#include <memory>
#include "ObjectContainer3D.h"
#include "Geometry.h"
#include "material/MaterialBase.h"

class Mesh : public ObjectContainer3D {
protected:
    std::shared_ptr<Geometry> _geometry;
    std::shared_ptr<MaterialBase> _material;
public:
    Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<MaterialBase> material) : _geometry(geometry), _material(material) {}
    ~Mesh() = default;

    MaterialBase* material() const { return _material.get(); }
    Geometry* geometry() const { return _geometry.get(); }

    std::shared_ptr<Geometry> GetGeometry() const { return _geometry; }
    std::shared_ptr<MaterialBase> GetMaterial() const { return _material; }

    void SetGeometry(std::shared_ptr<Geometry> geometry) { _geometry = geometry; }
    void SetMaterial(std::shared_ptr<MaterialBase> material) { _material = material; }
    glm::vec3& GetRotation() { return _rotation; }
};