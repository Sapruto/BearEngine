#include "GraphicsUtilits/UI/HierarchyViewerUI.h"

using namespace UtilitUI;

HierarchyViewerUI::HierarchyViewerUI(Canvas* cv, ResourceManager* resMgr) 
    : canvas(cv), resources(resMgr) {}

void HierarchyViewerUI::LoadResources() {
    if (!resources) return;
    
    resources->LoadResource(resource_path + "fonts/arial.ttf", ResourceType::Font);
    resources->LoadResource(resource_path + "ui/gameObjectpanel.png", ResourceType::Texture);

    objectFont = resources->GetResourceAs<Font>(resource_path + "fonts/arial.ttf");
    objectTextureButton = resources->GetResourceAs<Texture>(resource_path + "ui/gameObjectpanel.png");
}

Canvas* HierarchyViewerUI::GetCanvas() { return canvas; }

GameObject* HierarchyViewerUI::CreateUIObject(GameObject* gameObject, int index, Inspector* inspector) {
    if(!canvas) return nullptr;

    float y = yOffset + index * ySpacing;
    
    GameObject* obj = new GameObject(gameObject->GetName());
    
    obj->AddComponent<Tag>(AllTags::HierarchyViewerUI);
    
    Image* bg = UtilitUI::AddImage(obj, objectTextureButton, Vector2(x, y), Vector2(250, 30), anchoredMin, anchoredMax, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    canvas->AddUIElement(bg);
    bg->SetLayer(Layers::HierarchyButton);
    
    Button* btn = obj->AddComponent<Button>();
    btn->SetUIElement(bg);
    btn->SetCanvas(canvas);
    btn->Start();

    btn->Subscribe(Button::ClickState::JustPressed, [this, gameObject, inspector]() {
        std::cout << "CLICK ON BUTTON GAME OBJECT: " << gameObject->GetName() << std::endl;
        inspector->SetObject(gameObject);
    });
    
    if (objectFont) {
        Text* label = UtilitUI::AddTextWithAnchors(obj, objectFont, gameObject->GetName(), Vector2(x, y), Vector2(230, 20), anchoredMin, anchoredMax, glm::vec4(1.0f));
        canvas->AddUIElement(label);
        label->SetLayer(Layers::HierarchyText);
        label->SetVisible(false);
    }
    
    return obj;
}

void HierarchyViewerUI::UpdateUIObjectPosition(GameObject* obj, int index) {
    float y = yOffset + index * ySpacing;
    
    Image* bg = obj->GetComponentOfType<Image>();
    if (bg) {
        bg->rectTransform->SetAnchoredPosition(Vector2(x, y));
    }
    
    Text* label = obj->GetComponentOfType<Text>();
    if (label) {
        label->rectTransform->SetAnchoredPosition(Vector2(x, y));
    }
}

void HierarchyViewerUI::UpdateUIObjectText(GameObject* obj, const std::string& name) {
    Text* label = obj->GetComponentOfType<Text>();
    if (label) {
        label->SetText(name);
    }
}

void HierarchyViewerUI::DestroyUIObject(GameObject* obj, Scene* scene) {
    if (!obj) return;
    
    Image* bg = obj->GetComponentOfType<Image>();
    if (bg && canvas) {
        canvas->RemoveUIElement(bg);
    }
    
    Text* label = obj->GetComponentOfType<Text>();
    if (label && canvas) {
        canvas->RemoveUIElement(label);
    }
    
    Button* btn = obj->GetComponentOfType<Button>();
    if (btn) {
        btn->ClearAllSubscriptions();
    }

    scene->RemoveGameObject(obj);
}