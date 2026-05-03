/*#include "SceneFileController.h"

#include "Editor.h"

SceneFileController::SceneFileController(Editor* editor, const std::string& scenePath, InputSystem& input) 
    : editor(editor), scenePath(scenePath), input(input) {
    editorScene = std::make_unique<Scene>("EditorScene");
    editorScene->InitializeScene();
    editorScene->StartScene();
}

void SceneFileController::ApplyEditorObjectChange() {
    for (auto& [gameObj, editorObj] : editorCopyToGameObject) {
        auto editorComponents = editorObj->GetComponents();
        auto gameComponents = gameObj->GetComponents();
        
        for (auto* editorComp : editorComponents) {
            if (auto* editorSerializable = dynamic_cast<ISerializable*>(editorComp)) {
                for (auto* gameComp : gameComponents) {
                    if (typeid(*gameComp) == typeid(*editorComp)) {
                        if (auto* gameSerializable = dynamic_cast<ISerializable*>(gameComp)) {
                            auto editorFields = editorSerializable->GetSerializedFields();
                            
                            auto gameFields = gameSerializable->GetSerializedFields();
                            
                            for (auto* editorField : editorFields) {
                                for (auto* gameField : gameFields) {
                                    if (editorField->GetName() == gameField->GetName()) {
                                        gameField->FromString(editorField->ToString());
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void SceneFileController::SaveScene() {
    if (!currentGameScene) return;

    ApplyEditorObjectChange();
    
    std::string scenesPath = "Assets/scenes";
    std::string sceneFilePath = scenesPath + "/test_scene1.scene";
    bool saved = sceneCreator->UpdateSceneFile(sceneFilePath, *currentGameScene);
    
    if (saved) {
        std::cout << "Scene saved successfully to: " << sceneFilePath << std::endl;
        isDirty = false;
    } else {
        std::cout << "Failed to save scene!" << std::endl;
    }
}

void SceneFileController::Restart() {
    std::unique_ptr<Scene> loadedScene = sceneCreator->GetScene(scenePath);
    if (!loadedScene) return;

    allGameScenes.push_back(std::move(loadedScene));
    currentGameScene = allGameScenes.back().get();
    
    if(editor) editor->Restart();
    
    isDirty = false;
}

void SceneFileController::Start() {
    if (!editorScene) {
        editorScene = std::make_unique<Scene>("EditorScene");
        editorScene->InitializeScene();
        editorScene->StartScene();
    }

    if (!sceneCreator) {
        SceneSerializer serializer;
        SceneDeserializer deserializer;
        sceneCreator = std::make_unique<SceneCreator>(serializer, deserializer);
    }
    
    std::unique_ptr<Scene> loadedScene = sceneCreator->GetScene(scenePath);
    if (loadedScene) {
        allGameScenes.push_back(std::move(loadedScene));
        currentGameScene = allGameScenes.back().get();
    } 
    else {
        auto newScene = std::make_unique<Scene>("EmptyScene");
        newScene->InitializeScene();
        newScene->StartScene();
        allGameScenes.push_back(std::move(newScene));
        currentGameScene = allGameScenes.back().get();
    }
    
    Restart();
}

void SceneFileController::Update() {
    if (input.GetKeyDown(Keys::S) && input.GetKey(Keys::LeftControl)) {
        SaveScene();
    }

    editorScene->UpdateScene();
}

void SceneFileController::SetPath(const std::string& scenePath) { 
    this->scenePath = scenePath; 
    isDirty = true; 
}

GameObject* SceneFileController::CreateGameObject() {
    if (!currentGameScene) return nullptr;
    GameObject* gameObj = currentGameScene->CreateGameObject();
    GameObject* editorObj = editorScene->CreateGameObject();
    editorCopyToGameObject[gameObj] = editorObj;
    return editorObj;
}
GameObject* SceneFileController::CreateGameObject(const std::string& name) {
    if (!currentGameScene) return nullptr;

    GameObject* gameObj = currentGameScene->CreateGameObject();
    GameObject* editorObj = editorScene->CreateGameObject();
    gameObj->SetName(name);
    editorObj->SetName(name);

    editorCopyToGameObject[gameObj] = editorObj;
    return editorObj;
}

GameObject* SceneFileController::CreateHelperObject(AllTags::Type tag) {
    if (!editorScene) return nullptr;
    GameObject* helper = editorScene->CreateGameObject();
    return helper;
}

std::vector<GameObject*> SceneFileController::GetHelpersFromTag(AllTags::Type tag) {
    std::vector<GameObject*> result;
    if (!editorScene) return result;
    
    for (auto* obj : editorScene->GetGameObjects()) {
        if (auto* objTag = obj->GetComponentOfType<Tag>()) {
            if (objTag->GetTag() == tag) {
                result.push_back(obj);
            }
        }
    }
    return result;
}*/