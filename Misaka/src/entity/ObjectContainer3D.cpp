#include "ObjectContainer3D.h"

namespace Misaka {

ObjectContainer3D::~ObjectContainer3D() {
    for (auto child : _children) {
        if (child) {
            delete child;
        }
    }
    _children.clear();
}

void ObjectContainer3D::AddChild(ObjectContainer3D* child) {
    if (!child) return;
    if (child->_parent) {
        child->_parent->RemoveChild(child);
    }
    child->_parent = this;
    _children.push_back(child);
}

void ObjectContainer3D::RemoveChild(ObjectContainer3D* child) {
    auto it = std::remove(_children.begin(), _children.end(), child);
    if (it != _children.end()) {
        _children.erase(it, _children.end());
        child->_parent = nullptr;
    }
}

const std::vector<ObjectContainer3D*>& ObjectContainer3D::GetChildren() const {
    return _children;
}

const std::vector<ObjectContainer3D*> ObjectContainer3D::GetFlatChildren() const {
    std::vector<ObjectContainer3D*> flatList;
    for (auto child : _children) {
        flatList.push_back(child);
        auto subChildren = child->GetFlatChildren();
        flatList.insert(flatList.end(), subChildren.begin(), subChildren.end());
    }
    return flatList;
}

glm::mat4 ObjectContainer3D::GetWorldMatrix() {
    glm::mat4 localMatrix = GetModelMatrix();
    if (_parent) {
        return _parent->GetWorldMatrix() * localMatrix;
    }
    return localMatrix;
}

} // namespace Misaka
