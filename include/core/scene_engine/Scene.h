#pragma once

#include <memory> 
#include <vector>
#include <string>
#include <unordered_map>
#include "GameObject.h"
#include "HierarchySystem.h"
#include "ResourceManager.h"
#include "ResourcesTypes.h"
#include "SceneEvent.h"

class Component;

class Scene {
private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<GameObject*> objectsToRemove;
    std::vector<std::unique_ptr<SceneEvent>> sceneEvents;
    
    HierarchySystem* hierarchySystem{nullptr};
    ResourceManager* resourceManager{nullptr};
    
    std::unordered_map<std::string, ResourceType> resources;
    std::unordered_map<std::string, Component*> uuidToComponent;
    
    bool isUpdating = false;
    bool isDestroyed = false;
    bool isActive = true;
    bool isInitialized = false;
    
    std::string sceneName;
    bool isDirty = false;
    
    struct CachedQuery {
        std::type_index type;
        std::vector<GameObject*> results;
        bool valid = false;
    };
    std::vector<CachedQuery> queryCache;

public:
    Scene() = default;
    Scene(const std::string& name) : sceneName(name) {}
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;
    ~Scene() = default;

    GameObject* AddGameObject(std::unique_ptr<GameObject> gameObject);
    void AddGameObjects(std::vector<std::unique_ptr<GameObject>> objects);
    GameObject* CreateGameObject();
    void RemoveGameObject(GameObject* gameObject);
    void QueueRemoveGameObject(GameObject* gameObject);
    void ProcessRemovalQueue();

    template<typename T>
    std::vector<GameObject*> FindGameObjectsWithComponent() {
        std::type_index type = typeid(T);
        
        for (auto& cache : queryCache) {
            if (cache.type == type && cache.valid) {
                return cache.results;
            }
        }
        
        std::vector<GameObject*> result;
        for (const auto& obj : gameObjects) {
            if (obj && !obj->IsDestroyed() && obj->GetComponentOfType<T>() != nullptr) {
                result.push_back(obj.get());
            }
        }
        
        queryCache.push_back({type, result, true});
        return result;
    }
    
    void InvalidateCache() {
        for (auto& cache : queryCache) {
            cache.valid = false;
        }
    }
    
    const std::vector<GameObject*> GetGameObjects() const;

    void SetActive(bool active);
    void InitializeScene();
    void StartScene();
    void UpdateScene();
    void DestroyScene();
    
    void MarkDirty() { isDirty = true; }
    bool IsDirty() const { return isDirty; }
    void ClearDirty() { isDirty = false; }

    void SetName(const std::string& newName) { sceneName = newName; }
    std::string GetName() const { return sceneName; }
    
    bool IsActive() const { return isActive; }
    bool IsInitialized() const { return isInitialized; }
    bool IsDestroyed() const { return isDestroyed; }

    void SetHierarchySystem(HierarchySystem* system) { hierarchySystem = system; }
    void SetResourceManager(ResourceManager* manager) { resourceManager = manager; }
    
    HierarchySystem* GetHierarchySystem() { return hierarchySystem; }
    ResourceManager* GetResourceManager() { return resourceManager; }
    const HierarchySystem* GetHierarchySystem() const { return hierarchySystem; }
    const ResourceManager* GetResourceManager() const { return resourceManager; }

    void AddResources(const std::unordered_map<std::string, ResourceType>& newResources);
    void LoadResources();
    void ResetResources();
    
    void ClearComponentRegistry() { uuidToComponent.clear(); }
    void RegisterAllComponents();
    void RegisterComponent(Component* comp);
    void UnregisterComponent(Component* comp);
    
    template<typename T>
    T* FindComponentByUUID(const std::string& uuid) {
        auto it = uuidToComponent.find(uuid);
        if (it != uuidToComponent.end()) {
            return dynamic_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    void AddSceneEvent(std::unique_ptr<SceneEvent> event);
    void RemoveSceneEvent(SceneEvent* event);
    template<typename T>
    T* GetSceneEvent() {
        for (auto& event : sceneEvents) {
            if (auto* typedEvent = dynamic_cast<T*>(event.get())) {
                return typedEvent;
            }
        }
        return nullptr;
    }
};