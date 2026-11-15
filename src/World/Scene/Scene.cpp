#include "World/Scene/Scene.h"

#include <algorithm>
#include <utility>

Scene::Scene()
    : hierarchySystem(std::make_unique<HierarchySystem>())
{
    eventBus.SetOwner(this);
}

Scene::Scene(const std::string& name)
    : hierarchySystem(std::make_unique<HierarchySystem>())
    , sceneName(name)
{
    eventBus.SetOwner(this);
}

GameObject* Scene::AddGameObject(std::unique_ptr<GameObject> gameObject) {
    if (!gameObject) return nullptr;

    GameObject* rawPtr = gameObject.get();
    gameObject->SetScene(this);
    if (hierarchySystem) {
        gameObject->SetHierarchySystem(hierarchySystem.get());
    }

    if (isActive && isInitialized) {
        gameObject->Initialize();

        eventBus.OnAddObject(rawPtr);

        gameObject->Awake();
        gameObject->Start();
    }

    gameObjects.push_back(std::move(gameObject));
    finder.Invalidate();
    MarkDirty();
    return rawPtr;
}

void Scene::AddGameObjects(std::vector<std::unique_ptr<GameObject>> objects) {
    for (auto& obj : objects) {
        AddGameObject(std::move(obj));
    }
}

GameObject* Scene::CreateGameObject() {
    auto go = std::make_unique<GameObject>();

    go->SetScene(this);
    if (hierarchySystem) {
        go->SetHierarchySystem(hierarchySystem.get());
    }

    if (isInitialized) {
        go->Initialize();
    }

    return AddGameObject(std::move(go));
}

void Scene::RemoveGameObject(GameObject* gameObject) {
    if (!gameObject) return;

    auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
        [gameObject](const std::unique_ptr<GameObject>& go) {
            return go.get() == gameObject;
        });

    if (it != gameObjects.end()) {
        eventBus.OnRemoveObject(gameObject);

        (*it)->Destroy();
        gameObjects.erase(it);
        finder.Invalidate();
        MarkDirty();
    }
}

void Scene::QueueRemoveGameObject(GameObject* gameObject) {
    if (!gameObject) return;

    if (isUpdating) {
        objectsToRemove.push_back(gameObject);
    } else {
        RemoveGameObject(gameObject);
    }
}

void Scene::ProcessRemovalQueue() {
    for (auto* obj : objectsToRemove) {
        RemoveGameObject(obj);
    }
    objectsToRemove.clear();
}

const std::vector<GameObject*> Scene::GetGameObjects() const {
    std::vector<GameObject*> result;
    result.reserve(gameObjects.size());
    for (const auto& obj : gameObjects) {
        if (obj && !obj->IsDestroyed()) {
            result.push_back(obj.get());
        }
    }
    return result;
}

void Scene::SetActive(bool active) {
    if (isActive == active) return;

    isActive = active;

    if (active && !isInitialized) {
        InitializeScene();
        StartScene();
        isInitialized = true;
    }
}

void Scene::InitializeScene() {
    if (isInitialized) return;

    for (auto& gameObject : gameObjects) {
        if (!gameObject) continue;
        gameObject->SetScene(this);
        if (hierarchySystem) {
            gameObject->SetHierarchySystem(hierarchySystem.get());
        }
        gameObject->Initialize();
    }
}

void Scene::AwakeScene() {
    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            gameObject->Awake();
        }
    }
}

void Scene::StartScene() {
    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            gameObject->Start();
        }
    }
    eventBus.OnSceneStart();
}

void Scene::UpdateScene() {
    if (!isActive || isDestroyed) return;

    isUpdating = true;

    for (auto& gameObject : gameObjects) {
        if (gameObject && !gameObject->IsDestroyed()) {
            gameObject->Update();
        }
    }

    eventBus.OnSceneUpdate();

    isUpdating = false;

    ProcessRemovalQueue();

    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const std::unique_ptr<GameObject>& go) {
                return !go || go->IsDestroyed();
            }),
        gameObjects.end());

    finder.Invalidate();
}

void Scene::DestroyScene() {
    if (isDestroyed) return;

    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            eventBus.OnRemoveObject(gameObject.get());
            gameObject->Destroy();
        }
    }

    eventBus.OnSceneDestroy();

    gameObjects.clear();
    objectsToRemove.clear();
    componentCatalog.Clear();
    finder.Clear();

    isInitialized = false;
    isActive = false;
    isDestroyed = true;
}