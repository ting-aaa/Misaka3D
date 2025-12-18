#pragma once
#include <string>


enum class ResourceType
{
    Unknown = 0,
    Texture,
    Geometry,
    Material,
    ModelPrefab,
    Shader
};

class IResource{
protected:
    std::string _uid;
    std::string _name;
public:
    virtual ~IResource() = default;
    void SetUID(const std::string& uid){  _uid = uid; }
    std::string GetUID() const { return _uid; }

    void SetName(const std::string& name){  _name = name; }
    std::string GetName() const { return _name; }

    virtual ResourceType GetResourceType() const = 0;
};