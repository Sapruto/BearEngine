#pragma once

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <utility>
#include <string>

#include "include/core/system_engine/resource_system/Resource.h"

#include "include/core/system_engine/resource_system/ResourcesTypes.h"
#include "include/core/system_engine/resource_system/ResourceFactory.h"

class ResourceManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
    
    static ResourceManager* instance;
    ResourceManager() = default;
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
public:
    ~ResourceManager();

    static ResourceManager& GetInstance();
    static void DestroyInstance();
    
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