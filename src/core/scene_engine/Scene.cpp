#include "Scene.h"

GameObject* Scene::AddGameObject(std::unique_ptr<GameObject> gameObject){
    if (!gameObject) return nullptr;
    
    GameObject* rawPtr = gameObject.get();
    
    if (isActive && isInitialized){
        gameObject->Start();
    }
    
    gameObjects.push_back(std::move(gameObject));
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
    
    if (isInitialized) {
        go->SetHierarchySystem(&hierarchySystem);
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

void Scene::InitializeScene(){
    if (isInitialized) return;

    LoadResources(resources);

    for (auto& gameObject : gameObjects){
        gameObject->SetHierarchySystem(&hierarchySystem);
    }
}

void Scene::StartScene(){
    ProcessEvents(startEvents);
    
    for (auto& gameObject : gameObjects){
        if (gameObject) {
            gameObject->Start();
        }
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

void Scene::UpdateScene(){
    if (!isActive) return;
    
    ProcessEvents(updateEvents);
    
    isUpdating = true; 
    
    for(auto& gameObject : gameObjects){
        if(gameObject){
            gameObject->Update();
        }
    }
    
    isUpdating = false;
    
    ProcessRemovalQueue();
    
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const std::unique_ptr<GameObject>& go) {
                return !go;
            }),
        gameObjects.end()
    );
}

void Scene::DestroyScene() {
    if(!isInitialized) return;
    ProcessEvents(destroyEvents);
    
    for(auto& gameObject : gameObjects){
        if(gameObject){
            gameObject->Destroy();
        }
    }
    
    gameObjects.clear();
    isInitialized = false;
    isActive = false;
}

void Scene::ProcessEvents(std::vector<SceneEvent>& events){
    for(auto& event : events){
        if(event){
            event();
        }
    }
}

void Scene::AddSceneEvent(SceneEvent event, TypeSceneEvent typeEvent){
    if(typeEvent == TypeSceneEvent::toStart){
        startEvents.push_back(event);
    }
    else if(typeEvent == TypeSceneEvent::toUpdate){
        updateEvents.push_back(event);
    }
    else if(typeEvent == TypeSceneEvent::toDestroy){
        destroyEvents.push_back(event);
    }
}

const std::vector<GameObject*> Scene::GetGameObjects() const {
    std::vector<GameObject*> result;
    result.reserve(gameObjects.size());
    for (const auto& obj : gameObjects) {
        if (obj) {
            result.push_back(obj.get());
        }
    }
    return result;
}