#include "EditorToolsController.h"
#include "Editor.h"
#include "Scene.h"
#include "ResourceManager.h"

void EditorToolsController::UpdateLogic() {
    if (tools.hierarchyViewer) tools.hierarchyViewer->Update();
    if (tools.inspectorLogic) tools.inspectorLogic->Update();
    if (tools.sceneView) tools.sceneView->Update();
}
void EditorToolsController::UpdateUI() {
    if (tools.sceneView) tools.sceneView->Render();
}

bool EditorToolsController::Initialize(Editor* editor, GraphicsManager* gMgr, ResourceManager* resMgr, Scene* editorScene) {
    this->editor = editor;
    this->graphicsManager = gMgr;
    this->resources = resMgr;
    
    uiRenderer = graphicsManager->AddRender<UIRendering>();
    
    editorUI = new UtilitUI::EditorUI(uiRenderer, resources, editorScene);
    editorUI->LoadResources();
    
    editorCanvas = new Canvas(1600.0f, 900.0f);
    editorUI->CreateUI(editorCanvas);
    
    auto* hierarchyUI = new HierarchyViewerUI(editorCanvas, resources);
    hierarchyUI->LoadResources();
    toolsUI.hierarchyViewerUI.reset(hierarchyUI);
    
    auto* inspectorUI = new InspectorUI(editorCanvas, resources, editorScene);
    inspectorUI->LoadResources();
    toolsUI.inspectorUI.reset(inspectorUI);
    
    tools.hierarchyViewer = std::make_unique<HierarchyViewer>(editor, hierarchyUI);
    tools.hierarchyViewer->SetScene(editor->GetCurrentGameScene());
    tools.hierarchyViewer->Start();
    
    tools.inspectorLogic = std::make_unique<InspectorLogic>(editor->GetCurrentGameScene(), inspectorUI);
    
    uiRenderer->RegisterRenderComponent(editorCanvas);
    
    tools.sceneView = std::make_unique<SceneView>(editor->GetSceneFileController(), graphicsManager);
    tools.sceneView->Start();
    
    return true;
}

void EditorToolsController::Update() {
    UpdateLogic();
    UpdateUI();
}

void EditorToolsController::Destroy() {
    toolsUI.inspectorUI.reset();
    toolsUI.hierarchyViewerUI.reset();
    tools.inspectorLogic.reset();
    tools.hierarchyViewer.reset();
    tools.sceneView.reset();
    
    delete editorUI;
    delete editorCanvas;
}