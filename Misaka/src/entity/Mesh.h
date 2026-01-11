#pragma once

#include <memory>
#include "ObjectContainer3D.h"
#include "../renderer/Geometry.h"
#include "../material/MaterialBase.h"

namespace Misaka {

class Mesh : public ObjectContainer3D {
protected:
    std::shared_ptr<Geometry> _geometry;
    std::shared_ptr<MaterialBase> _material;

public:
    Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<MaterialBase> material);
    ~Mesh() = default;

    MaterialBase* material() const;
    Geometry* geometry() const;

    std::shared_ptr<Geometry> GetGeometry() const;
    std::shared_ptr<MaterialBase> GetMaterial() const;

    void SetGeometry(std::shared_ptr<Geometry> geometry);
    void SetMaterial(std::shared_ptr<MaterialBase> material);
    
    glm::vec3& GetRotation();
};

} // namespace Misaka
