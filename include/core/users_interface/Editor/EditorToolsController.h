#pragma once

#include "GraphicsUtilits/GraphicsUtilit.h"
#include "UtilitUI.h"
#include "Canvas.h"
#include "UIRendering.h"
#include "InputSystem.h"
#include <memory>

#include "InspectorLogic.h"
#include "InspectorUI.h"
#include "HierarchyViewer.h"
#include "HierarchyViewerUI.h"
#include "SceneView.h"

class SceneView;
class Editor;
class ResourceManager;
class Scene;

struct EditorTools {
    std::unique_ptr<InspectorLogic> inspectorLogic;
    std::unique_ptr<HierarchyViewer> hierarchyViewer;
    std::unique_ptr<SceneView> sceneView;
};

struct EditorToolsUI {
    std::unique_ptr<InspectorUI> inspectorUI;
    std::unique_ptr<HierarchyViewerUI> hierarchyViewerUI;
};

class EditorToolsController {
private:
    Editor* editor{nullptr};
    GraphicsManager* graphicsManager{nullptr};
    ResourceManager* resources{nullptr};
    UtilitUI::EditorUI* editorUI{nullptr};
    UIRendering* uiRenderer{nullptr};
    Canvas* editorCanvas{nullptr};

    EditorTools tools;
    EditorToolsUI toolsUI;

    void UpdateLogic();
    void UpdateUI();

public:
    EditorToolsController() = default;
    
    bool Initialize(Editor* editor, GraphicsManager* gMgr, ResourceManager* resMgr, Scene* editorScene);
    void Update();
    void Destroy();
    
    EditorTools* GetTools() { return &tools; }

    InspectorLogic* GetInspectorLogic() { return tools.inspectorLogic.get(); }
    Canvas* GetCanvas() { return editorCanvas; }
};