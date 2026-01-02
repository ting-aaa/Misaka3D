#pragma once

#include <string>
#include <filesystem>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

#include "IResource.h"

namespace Misaka {

class PathResolver {
public:
    static std::string Resolve(const std::string& rawPath, const std::string& modelPath);
};

class AssetManager {
public:
    static AssetManager* Ins() {
        static AssetManager instance;
        return &instance;
    }

    template<typename R>
    std::shared_ptr<R> GetOrLoad(const std::string& uid, std::function<std::shared_ptr<R>()> loader) {
        constexpr ResourceType type = R::TypeEnum;
        auto& groupCache = _resourceGroups[type];
        auto it = groupCache.find(uid);
        if (it != groupCache.end()) {
            return std::dynamic_pointer_cast<R>(it->second);
        }
        std::shared_ptr<R> res = loader();
        if (res) {
            res->SetUID(uid);
            if (res->GetResourceType() != type) {
                std::cerr << "Resource type mismatch for UID: " << uid << std::endl;
                return nullptr;
            }
            groupCache[uid] = res;
        } else {
            std::cerr << "Failed to load resource: " << uid << std::endl;
        }
        return res;
    }

    template<typename R>
    std::shared_ptr<R> Find(const std::string& uid) {
        constexpr ResourceType type = R::TypeEnum;
        auto& groupCache = _resourceGroups[type];
        
        auto it = groupCache.find(uid);
        if (it != groupCache.end()) {
            return std::dynamic_pointer_cast<R>(it->second);
        }
        return nullptr;
    }

    template<typename R>
    std::vector<std::shared_ptr<R>> GetAllAssets() {
        constexpr ResourceType type = R::TypeEnum;
        std::vector<std::shared_ptr<R>> result;
        
        auto& groupCache = _resourceGroups[type];
        for (auto& [uid, res] : groupCache) {
            result.push_back(std::dynamic_pointer_cast<R>(res));
        }
        return result;
    }

    void UnloadUnusedByType(ResourceType type);
    void UnloadAllUnused();
    void RegisterResource(std::shared_ptr<IResource> res);
    void Clear();
    
private:
    std::map<ResourceType, std::map<std::string, std::shared_ptr<IResource>>> _resourceGroups;
};

} // namespace Misaka
