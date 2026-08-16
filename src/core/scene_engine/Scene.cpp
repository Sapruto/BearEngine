#include "Scene.h"
#include "Component.h"
#include "SceneEvent.h"

GameObject* Scene::AddGameObject(std::unique_ptr<GameObject> gameObject){
    if (!gameObject) return nullptr;
    
    GameObject* rawPtr = gameObject.get();
    gameObject->SetScene(this);
    if (hierarchySystem) {
        gameObject->SetHierarchySystem(hierarchySystem);
    }
    
    if (isActive && isInitialized){
        gameObject->Start();
    }
    
    gameObjects.push_back(std::move(gameObject));
    InvalidateCache();
    MarkDirty();
    return rawPtr;
}

void Scene::AddGameObjects(std::vector<std::unique_ptr<GameObject>> objects){
    for (auto& obj : objects) {
        AddGameObject(std::move(obj));
    }
}

GameObject* Scene::CreateGameObject(){
    auto go = std::make_unique<GameObject>();
    GameObject* rawPtr = go.get();
    
    go->SetScene(this);
    if (hierarchySystem) {
        go->SetHierarchySystem(hierarchySystem);
    }
    
    if (isInitialized) {
        go->Initialize();
    }
    
    return AddGameObject(std::move(go));
}

void Scene::RemoveGameObject(GameObject* gameObject){
    if (!gameObject) return;
    
    auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
        [gameObject](const std::unique_ptr<GameObject>& go){
            return go.get() == gameObject;
        });
    
    if (it != gameObjects.end()){
        (*it)->Destroy();
        gameObjects.erase(it);
        InvalidateCache();
        MarkDirty();
    }
}

void Scene::SetActive(bool active){
    if (isActive == active) return;
    
    isActive = active;
    
    if (active && !isInitialized){
        InitializeScene();
        StartScene();
        isInitialized = true;
    }
}

void Scene::InitializeScene() {
    if (isInitialized) return;

    LoadResources();

    for (auto& gameObject : gameObjects) {
        gameObject->SetScene(this);
        if (hierarchySystem) {
            gameObject->SetHierarchySystem(hierarchySystem);
        }
        gameObject->Initialize();
    }
    
    for (auto& event : sceneEvents) {
        event->SetScene(this);
        event->OnSceneStart();
    }
}

void Scene::StartScene() {
    for (auto& gameObject : gameObjects){
        if (gameObject) {
            gameObject->Start();
        }
    }
    for (auto& event : sceneEvents) {
        event->OnSceneStart();
    }
}

void Scene::QueueRemoveGameObject(GameObject* gameObject){
    if(!gameObject) return;
    
    if(isUpdating){
        objectsToRemove.push_back(gameObject);
    } 
    else{
        RemoveGameObject(gameObject);
    }
}

void Scene::ProcessRemovalQueue(){
    for(auto* obj : objectsToRemove){
        RemoveGameObject(obj);
    }
    objectsToRemove.clear();
}

void Scene::UpdateScene() {
    if (!isActive || isDestroyed) return;
    
    isUpdating = true; 
    
    for(auto& gameObject : gameObjects) {
        if (!gameObject->IsDestroyed()) {
            gameObject->Update();
        }
    }
    
    for(auto& event : sceneEvents) {
        if (event && event->IsEnabled()) {
            event->OnSceneUpdate();
        }
    }
    
    isUpdating = false;
    
    ProcessRemovalQueue();
    
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const auto& go) { return go->IsDestroyed(); }),
        gameObjects.end()
    );
}

void Scene::DestroyScene() {
    if(!isInitialized) return;
    
    for(auto& event : sceneEvents) {
        event->OnSceneDestroy();
    }
    sceneEvents.clear();
    
    for(auto& gameObject : gameObjects){
        if(gameObject){
            gameObject->Destroy();
        }
    }
    
    gameObjects.clear();
    objectsToRemove.clear();
    uuidToComponent.clear();
    queryCache.clear();
    
    isInitialized = false;
    isActive = false;
    isDestroyed = true;
}

void Scene::AddResources(const std::unordered_map<std::string, ResourceType>& newResources) {
    resources.insert(newResources.begin(), newResources.end());
}

void Scene::LoadResources() {
    if (!resourceManager) return;
    
    for(auto& [resourcePath, resourceType] : resources){
        resourceManager->LoadResource(resourcePath, resourceType);
    }
}

void Scene::ResetResources(){
    if (!resourceManager) return;
    
    for(auto& [resourcePath, resourceType] : resources){
        resourceManager->UnloadResource(resourcePath);
    }
    resources.clear();
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

void Scene::RegisterAllComponents() {
    ClearComponentRegistry();
    
    for (const auto& obj : gameObjects) {
        if (!obj) continue;
        
        for (auto* comp : obj->GetComponents()) {
            if (comp) {
                RegisterComponent(comp);
            }
        }
    }
}

void Scene::RegisterComponent(Component* comp) {
    if (comp && !comp->GetUUID().empty()) {
        uuidToComponent[comp->GetUUID()] = comp;
    }
}

void Scene::UnregisterComponent(Component* comp) {
    if (comp && !comp->GetUUID().empty()) {
        uuidToComponent.erase(comp->GetUUID());
    }
}

void Scene::AddSceneEvent(std::unique_ptr<SceneEvent> event) {
    if (!event) return;
    
    event->SetScene(this);
    if (isInitialized) {
        event->OnSceneStart();
    }
    sceneEvents.push_back(std::move(event));
}

void Scene::RemoveSceneEvent(SceneEvent* event) {
    if (!event) return;
    
    auto it = std::find_if(sceneEvents.begin(), sceneEvents.end(),
        [event](const std::unique_ptr<SceneEvent>& e) {
            return e.get() == event;
        });
    
    if (it != sceneEvents.end()) {
        (*it)->OnSceneDestroy();
        sceneEvents.erase(it);
    }
}