#include "Editor.h"

#include "SceneDeserializer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "Canvas.h"
#include "UIRendering.h"
#include "InputSystem.h"

#include "Inspector.h"
#include "HierarchyViewer.h"

#include "Time.h"

#include "Font.h"

Editor::Editor() 
    : sceneDeserializer()
    , sceneManager(sceneDeserializer)
    , input(InputSystem::GetInstance()) 
{}

bool Editor::Start() {
    if (!UtilitGraphic::InitGraphics(&graphicsManager, resources)) {
        return false;
    }

    Font::InitFreeType();
    
    uiRenderer = graphicsManager.AddRender<UIRendering>();
    
    editorScene = new Scene("editorScene");
    editorScene->InitializeScene();
    editorScene->StartScene();
    
    currentGameScene = new Scene("GameScene");
    currentGameScene->InitializeScene();
    currentGameScene->StartScene();
    
    editorUI = new UtilitUI::EditorUI(uiRenderer, &resources, editorScene);
    editorUI->LoadResources();
    
    Canvas* editorCanvas = new Canvas(1600.0f, 900.0f);
    editorUI->CreateUI(editorCanvas);
    
    UtilitUI::HierarchyViewerUI* hierarchyUI = new UtilitUI::HierarchyViewerUI(editorCanvas, &resources);
    hierarchyUI->LoadResources();
    
    tools.hierarchyViewer = new HierarchyViewer(this, hierarchyUI);
    tools.hierarchyViewer->SetScene(currentGameScene);
    tools.hierarchyViewer->StartHierarchy();

    UtilitUI::InspectorUI* inspectorUI = new UtilitUI::InspectorUI(editorCanvas, &resources, editorScene);
    inspectorUI->LoadResources();
    
    tools.inspector = new Inspector(currentGameScene, inspectorUI);
    
    uiRenderer->RegisterRenderComponent(editorCanvas);
    
    input.Initialize(graphicsManager.GetWindow()->GetWindow());
    
    return true;
}

void Editor::Update() {
    Time::Tick();
    input.Update();

    Window* window = graphicsManager.GetWindow();
    if (!window) return;

    if (!window->GetWindow()) return;
    
    editorScene->UpdateScene();

    tools.hierarchyViewer->UpdateHierarchy();
    tools.inspector->UpdateInspectOfObject();

    window->Clear();
    graphicsManager.Update();
    
    if (uiRenderer) {
        uiRenderer->Update();
    }
    
    window->SwapBuffers();
}

void Editor::Destroy() {
    delete tools.inspector;
    delete tools.hierarchyViewer;
}

SceneManager* Editor::GetSceneManager() { 
    return &sceneManager; 
}

bool Editor::IsRunEditor() const { 
    return isRunEditor; 
}

void Editor::StopEditor() { 
    isRunEditor = false; 
}

Inspector* Editor::GetInspector() { 
    return tools.inspector; 
}

Scene* Editor::GetEditorScene(){
    return editorScene;
}

Scene* Editor::GetCurrentGameScene(){
    return currentGameScene;
}