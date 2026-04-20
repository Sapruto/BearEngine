#include "HierarchyViewer.h"
#include "GraphicsUtilits/UI/HierarchyViewerUI.h"
#include "GameObject.h"
#include "Scene.h"
#include "Button.h"
#include "Editor.h"
#include "Inspector.h"

#include "Tag.h"
#include "EditorTags.h"

HierarchyViewer::HierarchyViewer(Editor* editor, UtilitUI::HierarchyViewerUI* ui) 
    : editor(editor), ui(ui) {}

void HierarchyViewer::ClearHierarchy() {
    for (auto& [obj, data] : hierarchyItems) {
        if (data.uiObject) {
            ui->DestroyUIObject(data.uiObject);
        }
    }
    hierarchyItems.clear();
}

void HierarchyViewer::StartHierarchy() {
    if (!currentGameScene || !ui) return;
    
    ClearHierarchy();
    
    int index = 0;
    for (auto* gameObject : currentGameScene->GetGameObjects()) {
        GameObject* uiObject = ui->CreateUIObject(gameObject, index, editor->GetInspector());
        
        hierarchyItems[gameObject] = {uiObject};
        editor->GetEditorScene()->AddGameObject(std::unique_ptr<GameObject>(uiObject));
        index++;
    }
}

void HierarchyViewer::UpdateHierarchy() {
    if (!currentGameScene || !ui) return;
    
    static int updateCounter = 0;
    updateCounter++;
    if (updateCounter > 1000) {
        std::cout << "WARNING: HierarchyViewer::UpdateHierarchy possible infinite loop!" << std::endl;
        return;
    }
    
    size_t currentCount = currentGameScene->GetGameObjects().size();
    
    size_t realGameObjectCount = 0;
    for (auto* obj : currentGameScene->GetGameObjects()) {
        Tag* tag = obj->GetComponentOfType<Tag>();
        if (!tag || tag->GetTag() != AllTags::HierarchyViewerUI) {
            realGameObjectCount++;
        }
    }
    
    static size_t lastObjectCount = 0;
    
    if (realGameObjectCount != lastObjectCount) {
        std::cout << "Hierarchy changed, recreating UI. Count: " << realGameObjectCount << std::endl;
        ClearHierarchy();
        StartHierarchy();
        lastObjectCount = realGameObjectCount;
    } 
    else {
        int index = 0;
        for (auto* gameObject : currentGameScene->GetGameObjects()) {
            Tag* tag = gameObject->GetComponentOfType<Tag>();
            if (tag && tag->GetTag() == AllTags::HierarchyViewerUI) {
                continue;
            }
            
            auto it = hierarchyItems.find(gameObject);
            if (it != hierarchyItems.end()) {
                ui->UpdateUIObjectPosition(it->second.uiObject, index);
            }
            index++;
        }
    }
    
    updateCounter = 0;
}

void HierarchyViewer::SetScene(Scene* scene) { 
    currentGameScene = scene; 
}