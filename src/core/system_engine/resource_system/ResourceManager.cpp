#include "ResourceManager.h"

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <utility>
#include <string>

#include "Resource.h"

#include "Texture.h"
#include "Font.h"
#include "Audio.h"
#include "ResourcesTypes.h"
#include "ResourceFactory.h"


ResourceManager::~ResourceManager() { 
    ClearAll(); 
}

void ResourceManager::LoadResource(const std::string& pathName, ResourceType type) {
    if (resources.find(pathName) != resources.end()) return;
    
    std::unique_ptr<Resource> resource = ResourceFactory::create(type, pathName);
    if (!resource) return;
    
    if (!resource->Load()) return;
    
    resources.emplace(pathName, std::move(resource));
}

Resource* ResourceManager::GetResource(const std::string& pathName) {
    auto it = resources.find(pathName);
    if (it != resources.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> ResourceManager::GetLoadedResources() const { 
    std::vector<std::string> result;
    result.reserve(resources.size());
    
    for (const auto& [name, resource] : resources) {
        result.push_back(name);
    }
    
    return result;
}

bool ResourceManager::UnloadResource(const std::string& pathName) {
    auto it = resources.find(pathName);
    if (it != resources.end()) {
        it->second->Unload();
        resources.erase(it);
        return true;
    }
    return false;
}

void ResourceManager::ClearAll() {
    for (auto& [name, resource] : resources) {
        resource->Unload();
    }
    resources.clear();
}

size_t ResourceManager::GetResourceCount() const {
    return resources.size();
}