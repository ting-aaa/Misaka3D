#pragma once

#include "ObjectContainer3D.h"
#include "Geometry.h"
#include "material/MaterialBase.h"

class Mesh : public ObjectContainer3D {
protected:
    Geometry* _geometry;
    MaterialBase* _material;
public:
    Mesh(Geometry* geometry, MaterialBase* material) : _geometry(geometry), _material(material) {}

    MaterialBase* material() const { return _material; }
    Geometry* geometry() const { return _geometry; }

    glm::vec3& GetRotation() { return _rotation; }
};