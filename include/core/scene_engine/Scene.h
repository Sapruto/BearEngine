#pragma once

#include <memory> 
#include <vector>
#include <string>
#include "SceneEvent.h"
#include "GameObject.h"

#include "HierarchySystem.h"
#include "ResourceManager.h"

#include "ResourcesTypes.h"

class Component;

class Scene {
private:
    HierarchySystem  hierarchySystem;

    ResourceManager resourceManager;
    std::unordered_map<std::string, ResourceType> resources;

    std::vector<std::unique_ptr<GameObject>> gameObjects;

    std::vector<GameObject*> objectsToRemove;

    std::vector<SceneEvent> startEvents;
    std::vector<SceneEvent> updateEvents;
    std::vector<SceneEvent> destroyEvents;

    bool isUpdating = false;
    bool isDestroyed = false;
    bool isActive = true;  
    bool isInitialized = false;

    std::string sceneName;

    std::unordered_map<std::string, Component*> uuidToComponent;

public:
    Scene() = default;
    Scene(const std::string& name) : sceneName(name) {} 
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    GameObject* AddGameObject(std::unique_ptr<GameObject> gameObject);
    void AddGameObjects(std::vector<std::unique_ptr<GameObject>> objects);
    GameObject* CreateGameObject(); 

    void RemoveGameObject(GameObject* gameObject);
    void SetActive(bool active);

    void QueueRemoveGameObject(GameObject* gameObject);
    void ProcessRemovalQueue();
    
    template<typename T>
    std::vector<GameObject*> FindGameObjectsWithComponent() {
        std::vector<GameObject*> result;
        
        for (const auto& obj : gameObjects) {
            if (obj && !obj->IsDestroyed() && obj->GetComponentOfType<T>() != nullptr) {
                result.push_back(obj.get());
            }
        }
        
        return result;
    }
    const std::vector<GameObject*> GetGameObjects() const;

    void InitializeScene();

    void StartScene();
    void UpdateScene();
    void DestroyScene();

    void AddSceneEvent(SceneEvent event, TypeSceneEvent typeEvent);

    void SetName(const std::string& newName) { sceneName = newName; }
    std::string GetName() const { return sceneName; }

    void AddResources(const std::unordered_map<std::string, ResourceType>& newResources){
        resources.insert(newResources.begin(), newResources.end());
    }

    void LoadResources(const std::unordered_map<std::string, ResourceType>& loadResources){
        for(auto& [resourcePath, resourceType] : resources){
            resourceManager.LoadResource(resourcePath, resourceType);
        }
    }

    void ResetResources(){
        for(auto& [resourcePath, resourceType] : resources){
            resourceManager.UnloadResource(resourcePath);
        }

        resources.clear();
    }

    HierarchySystem* GetHierarchySystem() { return &hierarchySystem; }
    ResourceManager* GetResourceManager() { return &resourceManager; }

    const HierarchySystem* GetHierarchySystem() const { return &hierarchySystem; }
    const ResourceManager* GetResourceManager() const { return &resourceManager; }


    void ClearComponentRegistry() {
        uuidToComponent.clear();
    }
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

private:
    void ProcessEvents(std::vector<SceneEvent>& events); 
};