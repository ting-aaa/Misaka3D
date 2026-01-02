#include "AssetManager.h"

namespace Misaka {

std::string PathResolver::Resolve(const std::string& rawPath, const std::string& modelPath) {
    std::filesystem::path raw(rawPath);
    std::filesystem::path model(modelPath);
    std::string fileName = raw.filename().string();

    // 模型同级目录查找
    std::filesystem::path sibling = model.parent_path() / fileName;

    // 检查模型目录 textures 子目录
    std::filesystem::path tex1 = model.parent_path() / "textures" / fileName;
    if (std::filesystem::exists(tex1)) return tex1.string();
    
    // 检查模型目录 Textures 子目录
    std::filesystem::path tex2 = model.parent_path() / "Textures" / fileName;
    if (std::filesystem::exists(tex2)) return tex2.string();
    
    // 返回同级目录
    return rawPath;
}

void AssetManager::UnloadUnusedByType(ResourceType type) {
    auto& groupCache = _resourceGroups[type];
    for (auto it = groupCache.begin(); it != groupCache.end(); ) {
        if (it->second.use_count() == 1) {
            it = groupCache.erase(it);
        } else {
            ++it;
        }
    }
}

void AssetManager::UnloadAllUnused() {
    for (auto& [type, group] : _resourceGroups) {
        UnloadUnusedByType(type);
    }
}

void AssetManager::RegisterResource(std::shared_ptr<IResource> res) {
    if (!res) return;
    _resourceGroups[res->GetResourceType()][res->GetUID()] = res;
}

void AssetManager::Clear() {
    _resourceGroups.clear();
}

} // namespace Misaka
