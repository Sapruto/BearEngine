#pragma once

#include "GameObjectFinder.h"
#include "SceneEventBus.h"
#include "ComponentCatalog.h"

#include "../GameObject.h"
#include "../Hierarchy/HierarchySystem.h"
#include "SceneEvent.h"

#include <memory>
#include <vector>
#include <string>

class Component;

class Scene {
private:
    GameObjectFinder finder;
    SceneEventBus eventBus;
    ComponentCatalog componentCatalog;

    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<GameObject*> objectsToRemove;

    std::unique_ptr<HierarchySystem> hierarchySystem;

    bool isUpdating = false;
    bool isDestroyed = false;
    bool isActive = true;
    bool isInitialized = false;

    std::string sceneName;
    bool isDirty = false;

public:
    Scene();
    explicit Scene(const std::string& name);
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
        return finder.Find<T>([this]() {
            std::vector<GameObject*> result;
            for (const auto& obj : gameObjects) {
                if (obj && !obj->IsDestroyed() && obj->GetComponentOfType<T>() != nullptr) {
                    result.push_back(obj.get());
                }
            }
            return result;
        });
    }
    const std::vector<GameObject*> GetGameObjects() const;

    void SetActive(bool active);
    void InitializeScene();
    void AwakeScene();
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

    HierarchySystem* GetHierarchySystem() { return hierarchySystem.get(); }
    const HierarchySystem* GetHierarchySystem() const { return hierarchySystem.get(); }

    GameObjectFinder& GetFinder() { return finder; }
    SceneEventBus& GetEventBus() { return eventBus; }
    ComponentCatalog& GetCatalog() { return componentCatalog; }
};