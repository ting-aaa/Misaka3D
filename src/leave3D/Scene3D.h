#pragma once

#include <type_traits>
#include "Object3D.h"
#include "ObjectContainer3D.h"
#include "Mesh.h"

static_assert(std::is_polymorphic_v<Object3D>, "Object3D must stay polymorphic for dynamic_cast in Scene3D.");

class Scene3D {
protected:
    ObjectContainer3D* _root;
public:
    Scene3D() {
        _root = new ObjectContainer3D();
    }

    ~Scene3D() {
        delete _root;
    }

    ObjectContainer3D* GetRoot() {
        return _root;
    }

    /** 获取所有子节点中的Mesh对象的扁平列表 */
    std::vector<Mesh*> GetFlatMeshList() {
        std::vector<Mesh*> meshList;
        auto flatChildren = _root->GetFlatChildren();
        for(auto child : flatChildren) {
            Mesh* mesh = dynamic_cast<Mesh*>(child);
            if(mesh) {
                meshList.push_back(mesh);
            }
        }
        return meshList;
    }
};