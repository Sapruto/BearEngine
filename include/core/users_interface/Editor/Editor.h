#pragma once

#include "SceneDeserializer.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "GraphicsUtilits/GraphicsUtilit.h"
#include "GraphicsUtilits/UI/UtilitUI.h"
#include "Canvas.h"
#include "UIRendering.h"
#include "InputSystem.h"

class Inspector;
class HierarchyViewer;
class SceneCreator;
class SceneView;

struct EditorTools {
    Inspector* inspector = nullptr;
    HierarchyViewer* hierarchyViewer = nullptr;
    SceneView* sceneView = nullptr;
};

class Editor {
private:
    SceneDeserializer sceneDeserializer;
    SceneManager sceneManager;
    GraphicsManager graphicsManager;
    ResourceManager resources;

    Scene* editorScene;
    Scene* currentGameScene;

    SceneCreator* sceneCreator;
    
    UtilitUI::EditorUI* editorUI;
    UIRendering* uiRenderer = nullptr;

    InputSystem& input;
    
    bool isRunEditor = true;

    EditorTools tools;

public:
    Editor();
    bool Start();
    void Update();
    void Destroy();

    SceneManager* GetSceneManager();
    bool IsRunEditor() const;
    void StopEditor();

    Inspector* GetInspector();

    Scene* GetEditorScene();
    Scene* GetCurrentGameScene();

    GraphicsManager* GetGraphicsManager() { return &graphicsManager; }

    void SetGameScene(std::unique_ptr<Scene> scene) {
        currentGameScene = scene.release();
    }
};