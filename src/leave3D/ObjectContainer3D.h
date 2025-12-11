#pragma once

#include <vector>
#include <algorithm> // remove
#include "Object3D.h"

class ObjectContainer3D : public Object3D {
protected:
    std::vector<ObjectContainer3D*> _children;
    ObjectContainer3D* _parent = nullptr; // 初始化 nullptr
public:
    // [修复] 虚析构函数，负责清理子节点
    virtual ~ObjectContainer3D() {
        for (auto child : _children) {
            // 确保子节点存在且不重复删除（虽然这里拥有权是唯一的，但安全起见）
            if (child) {
                delete child;
            }
        }
        _children.clear();
    }

    void AddChild(ObjectContainer3D* child) {
        if (!child) return;
        // 如果已经有父节点，先从原父节点移除
        if (child->_parent) {
            child->_parent->RemoveChild(child);
        }
        child->_parent = this;
        _children.push_back(child);
    }

    void RemoveChild(ObjectContainer3D* child) {
        auto it = std::remove(_children.begin(), _children.end(), child);
        if (it != _children.end()) {
            _children.erase(it, _children.end());
            child->_parent = nullptr;
        }
    }

    const std::vector<ObjectContainer3D*>& GetChildren() const {
        return _children;
    }

    /**
     * 获取所有子节点的扁平列表
     */
    const std::vector<ObjectContainer3D*> GetFlatChildren() const {
        std::vector<ObjectContainer3D*> flatList;
        for (auto child : _children) {
            flatList.push_back(child);
            auto subChildren = child->GetFlatChildren();
            flatList.insert(flatList.end(), subChildren.begin(), subChildren.end());
        }
        return flatList;
    }

    glm::mat4 GetWorldMatrix() {
        glm::mat4 localMatrix = GetModelMatrix();
        if (_parent) {
            return _parent->GetWorldMatrix() * localMatrix;
        }
        return localMatrix;
    }
};