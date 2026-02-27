#pragma once

#include <memory> 
#include <vector>
#include "include/core/scene_engine/SceneEvent.h"
#include "include/core/GameObject.h"
#include "include/core/system_engine/component_system/Component.h"

class Scene{
private:
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
    const std::vector<std::unique_ptr<GameObject>>* GetGameObjects() const;

    void StartScene();
    void UpdateScene();
    void DestroyScene();

    void AddSceneEvent(SceneEvent event, TypeSceneEvent typeEvent);

    void SetName(const std::string& newName) { sceneName = newName; }
    std::string GetName() const { return sceneName; }

private:
    void ProcessEvents(std::vector<SceneEvent>& events); 
};