#include "Systems/Resource/ResourceSystem.h"

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <utility>
#include <string>

#include "Systems/Resource/Resource.h"

#include "Systems/Resource/ResourcesTypes.h"
#include "Systems/Resource/ResourceFactory.h"


ResourceSystem::~ResourceSystem() { 
    ClearAll(); 
}

void ResourceSystem::LoadResource(const std::string& pathName, ResourceType type) {
    if (resources.find(pathName) != resources.end()) return;
    
    std::unique_ptr<Resource> resource = ResourceFactory::create(type, pathName);
    if (!resource) return;
    
    if (!resource->Load()) return;
    
    resources.emplace(pathName, std::move(resource));
}

Resource* ResourceSystem::GetResource(const std::string& pathName) {
    auto it = resources.find(pathName);
    if (it != resources.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> ResourceSystem::GetLoadedResources() const { 
    std::vector<std::string> result;
    result.reserve(resources.size());
    
    for (const auto& [name, resource] : resources) {
        result.push_back(name);
    }
    
    return result;
}

bool ResourceSystem::UnloadResource(const std::string& pathName) {
    auto it = resources.find(pathName);
    if (it != resources.end()) {
        it->second->Unload();
        resources.erase(it);
        return true;
    }
    return false;
}

void ResourceSystem::ClearAll() {
    for (auto& [name, resource] : resources) {
        resource->Unload();
    }
    resources.clear();
}

size_t ResourceSystem::GetResourceCount() const {
    return resources.size();
}