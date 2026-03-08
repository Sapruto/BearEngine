#pragma once

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <utility>
#include <string>

#include "Resource.h"

#include "ResourcesTypes.h"
#include "ResourceFactory.h"

class ResourceManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
    
public:
    ResourceManager() = default;
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ~ResourceManager();
    
    void LoadResource(const std::string& pathName, ResourceType type);
    
    Resource* GetResource(const std::string& pathName);
    
    template<typename T>
    T* GetResourceAs(const std::string& pathName) {
        return dynamic_cast<T*>(GetResource(pathName));
    }
    
    std::vector<std::string> GetLoadedResources() const;
    
    bool UnloadResource(const std::string& pathName);
    
    void ClearAll();
    
    size_t GetResourceCount() const;
};