#pragma once

#include <vector>
#include <algorithm>
#include "Object3D.h"

namespace Misaka {

class ObjectContainer3D : public Object3D {
protected:
    std::vector<ObjectContainer3D*> _children;
    ObjectContainer3D* _parent = nullptr;

public:
    virtual ~ObjectContainer3D();

    void AddChild(ObjectContainer3D* child);
    void RemoveChild(ObjectContainer3D* child);

    const std::vector<ObjectContainer3D*>& GetChildren() const;
    const std::vector<ObjectContainer3D*> GetFlatChildren() const;

    glm::mat4 GetWorldMatrix();
};

} // namespace Misaka
