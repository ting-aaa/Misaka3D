#pragma once
#include <string>
#include <filesystem>
#include <map>
#include <memory>
#include <functional>

#include "resources/IResource.h"


class PathResolver {
public:
    static std::string Resolve(const std::string& rawPath, const std::string& modelPath){
        std::filesystem::path raw(rawPath);
        std::filesystem::path model(modelPath);
        std::string fileName = raw.filename().string();

        //模型同级目录查找
        std::filesystem::path sibling = model.parent_path() / fileName;
        // if (std::filesystem::exists(sibling)) return sibling.string();

        // 检查模型目录 textures 子目录
        std::filesystem::path texturesDir = model.parent_path() / "textures" / fileName;
        if (std::filesystem::exists(texturesDir)) return texturesDir.string();
        // 检查模型目录 Textures 子目录
        std::filesystem::path texturesDir = model.parent_path() / "Textures" / fileName;
        if (std::filesystem::exists(texturesDir)) return texturesDir.string();
        
        // 返回同级目录
        return rawPath;
    }
};

class AssetManager {
public:
    static AssetManager* Ins() {
        if (!_instance) {
            _instance = new AssetManager();
        }
        return _instance;
    }

    template<typename R>
    std::shared_ptr<R> GetOrLoad(const std::string& uid, std::function<std::shared_ptr<R>()> loader){
        constexpr ResourceType type = R::TypeEnum;
        auto& groupCache = _resourceGroups[type];
        auto it = groupCache.find(uid);
        if (it != groupCache.end()) {
            return std::dynamic_pointer_cast<R>(it->second);
        }
        std::shared_ptr<R> res = loader();
        if (res) {
            res->SetUID(uid);
            if( res->GetResourceType() != type) {
                std::cerr << "Resource type mismatch for UID: " << uid << std::endl;
                return nullptr;
            }
            groupCache[uid] = res;
        }else{
            std::cerr << "Failed to load resource: " << uid << std::endl;
        }
        return res;
    }

    template<typename R>
    std::shared_ptr<R> Find( const std::string& uid ) {
        constexpr ResourceType type = R::TypeEnum;
        auto& groupCache = _resourceGroups[type];
        
        auto it = groupCache.find(uid);
        if (it != groupCache.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> GetAllAssets() {
        constexpr ResourceType type = T::TypeEnum;
        std::vector<std::shared_ptr<T>> result;
        
        auto& groupCache = _resourceGroups[type];
        for (auto& [uid, res] : groupCache) {
            result.push_back(std::dynamic_pointer_cast<T>(res));
        }
        return result;
    }

    // 卸载某类所有未使用的资源 (GC)
    void UnloadUnusedByType(ResourceType type) {
        auto& groupCache = _resourceGroups[type];
        for (auto it = groupCache.begin(); it != groupCache.end(); ) {
            if (it->second.use_count() == 1) {
                // std::cout << "[AssetManager] GC: " << it->first << std::endl;
                it = groupCache.erase(it);
            } else {
                ++it;
            }
        }
    }

    // 卸载所有类型的未使用资源
    void UnloadAllUnused() {
        for (auto& [type, group] : _resourceGroups) {
            UnloadUnusedByType(type);
        }
    }

    void RegisterResource( std::shared_ptr<IResource> res ){
        if (!res) return;
        _resourceGroups[res->GetResourceType()][res->GetUID()] = res;
    }

    void Clear(){
        _resourceGroups.clear();
    }
private:
    std::map<ResourceType, std::map<std::string, std::shared_ptr<IResource>>> _resourceGroups;
    static AssetManager* _instance;
};