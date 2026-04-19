#pragma once

#include "GraphicsUtilits/UI/UtilitUI.h"
#include "Component.h"

#include <memory>

class GameObject;
class Scene;
class Button;
class HierarchySystem;
class Editor;

class HierarchyViewer {
private:
    Editor* editor;
    UtilitUI::HierarchyViewerUI* ui;
    Scene* currentGameScene{nullptr};
    
    struct ItemData {
        GameObject* uiObject;
        Button* button;
        Text* label;
    };
    std::unordered_map<GameObject*, ItemData> hierarchyItems;

    void ClearHierarchy();

public:
    HierarchyViewer(Editor* editor, UtilitUI::HierarchyViewerUI* ui);

    void StartHierarchy();
    void UpdateHierarchy();

    void SetScene(Scene* scene);
};