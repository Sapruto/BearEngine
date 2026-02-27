#include "include/core/system_engine/resource_system/ResourceManager.h"

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <utility>
#include <string>

#include "include/core/system_engine/resource_system/Resource.h"

#include "include/core/system_engine/resource_system/ResourcesTypes/Texture.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Font.h"
#include "include/core/system_engine/resource_system/ResourcesTypes/Audio.h"
#include "include/core/system_engine/resource_system/ResourcesTypes.h"
#include "include/core/system_engine/resource_system/ResourceFactory.h"

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::~ResourceManager() { 
    ClearAll(); 
    instance = nullptr;
}

ResourceManager& ResourceManager::GetInstance() {
    if (!instance) {
        instance = new ResourceManager();
    }
    return *instance;
}
void ResourceManager::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
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