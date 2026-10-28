#pragma once

#include "../Core/ISystem.h"

#include "ResourcesTypes.h"
#include "Resource.h"

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

class ResourceSystem : public Systems::BaseSystem<ResourceSystem> {
private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
    
public:
    ResourceSystem() = default;
    
    ResourceSystem(const ResourceSystem&) = delete;
    ResourceSystem& operator=(const ResourceSystem&) = delete;
    ~ResourceSystem();
    
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

    static std::string_view GetSystemNameStatic() { return std::string_view("resource_system"); }
};