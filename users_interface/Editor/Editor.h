#pragma once

#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "InputSystem.h"

#include "SceneFileController.h"
#include "EditorToolsController.h"

class InspectorLogic;

class Editor {
private:
    GraphicsManager graphicsManager;
    ResourceManager resources;
    InputSystem& input;
    
    SceneFileController sceneController;
    EditorToolsController toolsController;
    
    bool isRunning{true};

public:
    Editor();
    
    bool Start();
    void Update();
    void Destroy() {}
    void Restart() {}
    
    Scene* GetCurrentGameScene();

    SceneFileController* GetSceneFileController() { return &sceneController; }

    bool IsRunEditor() { return isRunning; }

    //to delete
    Scene* GetEditorScene() { return sceneController.GetEditorScene(); }
    InspectorLogic* GetInspectorLogic() { return toolsController.GetInspectorLogic(); }
};