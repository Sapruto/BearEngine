#pragma once

#include "Scene.h"
#include "SceneManager.h"
#include "GraphicsManager.h"

#include "UtilitUI.h"

class Editor{
private:
    Scene editorScene;

    GraphicsManager graphicsManager;

    ResourceManager resources;

    bool isRunEditor{true};

public:
    void Start(std::string path){
        UtilitUI::InitGraphics(&graphicsManager, &editorScene, resources);
    }
    void Update(){
        editorScene.UpdateScene();
    }

    SceneManager* GetSceneManager() { return &sceneManager; }

    bool IsRunEditor() { return isRunEditor; }
};