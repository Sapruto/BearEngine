#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

#include "InputSystem.h"
#include "Keys.h"
#include "Scene.h"
#include "SceneCreator.h"
#include "SceneView.h"
#include "Tag.h"
#include "EditorTags.h"

class Editor;

class SceneFileController {
private:
    InputSystem& input;

    Editor* editor{nullptr};
    
    SceneDeserializer sceneDeserializer;
    std::vector<std::unique_ptr<Scene>> allGameScenes;

    std::unique_ptr<Scene> editorScene;
    Scene* currentGameScene{nullptr};

    std::unique_ptr<SceneCreator> sceneCreator;

    std::string scenePath;

    std::unordered_map<GameObject*, GameObject*> editorCopyToGameObject;
    std::unordered_map<std::string, GameObject*> helperObjects;

    std::unordered_map<std::string, Scene*> hotScenes;

    bool isDirty{false};

    void ApplyEditorObjectChange();
    void SaveScene();
    void Restart();

public:
    SceneFileController(Editor* editor, const std::string& scenePath, InputSystem& input);
    SceneFileController(const std::string& scenePath, InputSystem& input);

    void Start();
    void Update();
    void SetPath(const std::string& scenePath);

    GameObject* CreateGameObject();
    GameObject* CreateGameObject(const std::string& name);

    GameObject* CreateHelperObject(AllTags::Type tag);
    std::vector<GameObject*> GetHelpersFromTag(AllTags::Type tag);
    
    Scene* GetCurrentGameScene() { return currentGameScene; }
    Scene* GetEditorScene() { return editorScene.get(); }
    
    void RegisterAvatar(GameObject* gameObj, GameObject* avatarObj) {
        editorCopyToGameObject[gameObj] = avatarObj;
    }
    
    GameObject* GetAvatar(GameObject* gameObj) {
        return editorCopyToGameObject[gameObj];
    }
};