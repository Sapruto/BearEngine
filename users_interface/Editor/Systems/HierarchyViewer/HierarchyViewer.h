#pragma once

#include "HierarchyViewerUI.h"
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
    HierarchyViewerUI* ui;
    Scene* currentGameScene{nullptr};
    
    struct ItemData {
        GameObject* uiObject;
        Button* button;
        Text* label;
    };
    std::unordered_map<GameObject*, ItemData> hierarchyItems;

    void ClearHierarchy();

public:
    HierarchyViewer(Editor* editor, HierarchyViewerUI* ui);

    void Start();
    void Update();

    void SetScene(Scene* scene);
};