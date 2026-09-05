#pragma once

#include "UIRendering.h"
#include "Canvas.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Texture.h"
#include "Image.h"
#include "Button.h"
#include "GameObject.h"
#include "Text.h"
#include "Scene.h"

#include "UtilData/DrawComponentData.h"

#include "Tag.h"
#include "EditorTags.h"

#include "UtilitUI.h"

#include "InspectorLogic.h"

#include "UILayers.h"

#include "InputSystem.h"

class InspectorUI{
private:
    Canvas* canvas{nullptr};
    Font* inspectorFont{nullptr};
    Texture* buttonTexture{nullptr};
    Texture* inputBgTexture{nullptr};
    Texture* componentBgTexture{nullptr};

    Scene* scene{nullptr};
    ResourceManager* resources{nullptr};

    GameObject* activeInputObject{nullptr};

    bool isLoaded{false};

    float minScroll;
    float maxScroll;
    float yScroll;
    float speedScroll;
    float lastScroll;

    std::vector<GameObject*> headerObjects;
    std::vector<GameObject*> componentObjects;
    std::vector<GameObject*> componentListObjects;
    std::vector<GameObject*> settingsObjects;

    std::function<void()> onAddComponentCallback;

    Vector2 basePosition{760, -670};
    Vector2 baseAnchorMin{0.5f, 0.5f};
    Vector2 baseAnchorMax{0.5f, 0.5f};

    // Вспомогательная функция для преобразования glm::vec4 в массив
    void Vec4ToArray(const glm::vec4& v, float out[4]) {
        out[0] = v.r;
        out[1] = v.g;
        out[2] = v.b;
        out[3] = v.a;
    }

public:
    InspectorUI(Canvas* cv, ResourceManager* resMgr, Scene* scn)
        : canvas(cv), resources(resMgr), scene(scn), minScroll(0), maxScroll(0), yScroll(0), speedScroll(20), lastScroll(0) {}

    void SetBasePosition(const Vector2& pos) { basePosition = pos; }
    void SetBaseAnchors(const Vector2& min, const Vector2& max) { baseAnchorMin = min; baseAnchorMax = max; }

    void LoadResources() {
        if (!resources) return;
        
        resources->LoadResource("Assets/Base/fonts/arial.ttf", ResourceType::Font);
        inspectorFont = resources->GetResourceAs<Font>("Assets/Base/fonts/arial.ttf");
        
        resources->LoadResource("Assets/Base/ui/button.png", ResourceType::Texture);
        resources->LoadResource("Assets/Base/ui/input_bg.png", ResourceType::Texture);
        resources->LoadResource("Assets/Base/ui/component_bg.png", ResourceType::Texture);
        
        buttonTexture = resources->GetResourceAs<Texture>("Assets/Base/ui/button.png");
        inputBgTexture = resources->GetResourceAs<Texture>("Assets/Base/ui/input_bg.png");
        componentBgTexture = resources->GetResourceAs<Texture>("Assets/Base/ui/component_bg.png");
        
        if (!inputBgTexture) inputBgTexture = buttonTexture;
        if (!componentBgTexture) componentBgTexture = buttonTexture;

        if(!inspectorFont || !buttonTexture || !inputBgTexture || !componentBgTexture) return;

        isLoaded = true;
    }

    void BeginDraw() {
    }

    void ClearAll() {
        ClearHeaderObjects();
        ClearComponentObjects();
        ClearComponentListObjects();
        ClearSettingsObjects();
    }
    
    void ClearHeaderObjects() {
        for(auto* obj : headerObjects) {
            if(obj) DestroyUIObject(obj);
        }
        headerObjects.clear();
    }
    
    void ClearComponentObjects() {
        for(auto* obj : componentObjects) {
            if(obj) DestroyUIObject(obj);
        }
        componentObjects.clear();
    }
    
    void ClearComponentListObjects() {
        for(auto* obj : componentListObjects) {
            if(obj) DestroyUIObject(obj);
        }
        componentListObjects.clear();
    }
    
    void ClearSettingsObjects() {
        for(auto* obj : settingsObjects) {
            if(obj) DestroyUIObject(obj);
        }
        settingsObjects.clear();
    }

    void CreateHeader(GameObject* currentGameObject) {
        if (!currentGameObject || !scene || !isLoaded) return;
        
        ClearHeaderObjects();
        
        float color1[4] = {0.3f, 0.3f, 0.35f, 1.0f};
        float color2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float color3[4] = {0.2f, 0.6f, 0.2f, 1.0f};
        
        GameObject* headerGO = scene->CreateGameObject();
        headerObjects.push_back(headerGO);
        
        Image* headerBg = UtilitUI::AddImage(headerGO, buttonTexture,
            Vector2(basePosition.x - 300, basePosition.y + 500), Vector2(400, 40),
            baseAnchorMin, baseAnchorMax,
            color1);
        headerBg->SetLayer(UtilitUI::Layers::InspectorButton);
        canvas->AddUIElement(headerBg);
        
        Text* headerText = UtilitUI::AddTextWithAnchors(headerGO, inspectorFont, 
            "Inspector: " + currentGameObject->GetName(),
            Vector2(basePosition.x - 300, basePosition.y + 505), Vector2(380, 30),
            baseAnchorMin, baseAnchorMax,
            color2);
        headerText->SetLayer(UtilitUI::Layers::InspectorText);
        headerText->SetVisible(false);
        canvas->AddUIElement(headerText);
        
        GameObject* addBtnGO = scene->CreateGameObject();
        headerObjects.push_back(addBtnGO);
        
        Image* addBtnBg = UtilitUI::AddImage(addBtnGO, buttonTexture,
            Vector2(basePosition.x - 300, basePosition.y + 450), Vector2(150, 30),
            baseAnchorMin, baseAnchorMax,
            color3);
        addBtnBg->SetLayer(UtilitUI::Layers::InspectorButton);
        canvas->AddUIElement(addBtnBg);
        
        Text* addBtnText = UtilitUI::AddTextWithAnchors(addBtnGO, inspectorFont, "+ Add Component",
            Vector2(basePosition.x - 300, basePosition.y + 455), Vector2(140, 25),
            baseAnchorMin, baseAnchorMax,
            color2);
        addBtnText->SetLayer(UtilitUI::Layers::InspectorText);
        addBtnText->SetVisible(false);
        canvas->AddUIElement(addBtnText);
        
        Button* addButton = addBtnGO->AddComponent<Button>();
        addButton->SetUIElement(addBtnBg);
        addButton->SetCanvas(canvas);

        addButton->Subscribe(Button::ClickState::JustPressed, [this]() {
            if (onAddComponentCallback) {
                onAddComponentCallback();
            }
        });
    }

    void ShowComponentList(std::vector<std::string>& componentNames,
                            std::function<void(const std::string&)> onAddComponent) {
        if (!scene || !isLoaded) return;
        
        ClearComponentListObjects();
        
        float color1[4] = {0.3f, 0.3f, 0.4f, 1.0f};
        float color2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        
        float yOffset = basePosition.y + 200;
        float startX = basePosition.x - 150;
        
        for (size_t i = 0; i < componentNames.size(); i++) {
            GameObject* compBtnGO = scene->CreateGameObject();
            componentListObjects.push_back(compBtnGO);
            
            Image* btnBg = UtilitUI::AddImage(compBtnGO, buttonTexture,
                Vector2(startX, yOffset + (i * 40)), Vector2(300, 35),
                baseAnchorMin, baseAnchorMax,
                color1);
            btnBg->SetLayer(UtilitUI::Layers::InspectorButton);
            canvas->AddUIElement(btnBg);
            
            Text* btnText = UtilitUI::AddTextWithAnchors(compBtnGO, inspectorFont, componentNames[i],
                Vector2(startX + 10, yOffset + (i * 40) + 8), Vector2(280, 25),
                baseAnchorMin, baseAnchorMax,
                color2);
            btnText->SetLayer(UtilitUI::Layers::InspectorText);
            btnText->SetVisible(false);
            canvas->AddUIElement(btnText);
            
            Button* button = compBtnGO->AddComponent<Button>();
            button->SetUIElement(btnBg);
            button->SetCanvas(canvas);
            
            std::string compName = componentNames[i];
            button->Subscribe(Button::ClickState::JustPressed, [this, compName, onAddComponent]() {
                onAddComponent(compName);
            });
        }
    }

    void CreateObjectComponents(std::unordered_map<GameObject*, DrawComponentData>& inspectorObjects,
                                std::function<void(GameObject*, const std::string&)> onRemoveComponent) {
        if (!scene || !isLoaded) return;
        
        ClearComponentObjects();
        
        float color1[4] = {0.25f, 0.25f, 0.3f, 1.0f};
        float color2[4] = {0.9f, 0.9f, 1.0f, 1.0f};
        float color3[4] = {0.7f, 0.2f, 0.2f, 1.0f};
        float color4[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float color5[4] = {0.7f, 0.7f, 0.7f, 1.0f};
        float color6[4] = {0.15f, 0.15f, 0.2f, 1.0f};
        
        float yOffset = basePosition.y + 400;
        for (auto& [obj, data] : inspectorObjects) {
            if (obj == activeInputObject) continue;
            
            componentObjects.push_back(obj);
            
            Image* compBg = UtilitUI::AddImage(obj, componentBgTexture,
                Vector2(basePosition.x - 300, yOffset), Vector2(400, 80),
                baseAnchorMin, baseAnchorMax,
                color1);
            compBg->SetLayer(UtilitUI::Layers::InspectorButton);
            canvas->AddUIElement(compBg);
            
            Text* compName = UtilitUI::AddTextWithAnchors(obj, inspectorFont, data.componentName,
                Vector2(basePosition.x - 280, yOffset + 25), Vector2(200, 25),
                baseAnchorMin, baseAnchorMax,
                color2);
            compName->SetLayer(UtilitUI::Layers::InspectorText);
            compName->SetVisible(false);
            canvas->AddUIElement(compName);
            
            GameObject* removeBtnGO = scene->CreateGameObject();
            
            Image* removeBg = UtilitUI::AddImage(removeBtnGO, buttonTexture,
                Vector2(basePosition.x + 50, yOffset + 25), Vector2(60, 25),
                baseAnchorMin, baseAnchorMax,
                color3);
            removeBg->SetLayer(UtilitUI::Layers::InspectorButton);
            canvas->AddUIElement(removeBg);
            
            Text* removeText = UtilitUI::AddTextWithAnchors(removeBtnGO, inspectorFont, "X",
                Vector2(basePosition.x + 50, yOffset + 28), Vector2(60, 20),
                baseAnchorMin, baseAnchorMax,
                color4);
            removeText->SetLayer(UtilitUI::Layers::InspectorText);
            removeText->SetVisible(false);
            canvas->AddUIElement(removeText);
            
            Button* removeButton = removeBtnGO->AddComponent<Button>();
            removeButton->SetUIElement(removeBg);
            removeButton->SetCanvas(canvas);
            
            removeButton->Subscribe(Button::ClickState::JustPressed, [this, obj, onRemoveComponent, data]() {
                onRemoveComponent(obj, data.componentName);
            });
            
            float paramY = yOffset - 10;
            for (auto& [paramName, paramValue] : data.nameToParams) {
                paramY -= 30;
                
                Text* paramText = UtilitUI::AddTextWithAnchors(obj, inspectorFont, paramName + ":",
                    Vector2(basePosition.x - 280, paramY), Vector2(120, 25),
                    baseAnchorMin, baseAnchorMax,
                    color5);
                paramText->SetLayer(UtilitUI::Layers::InspectorText);
                paramText->SetVisible(false);
                canvas->AddUIElement(paramText);
                
                GameObject* valueBtnGO = scene->CreateGameObject();
                
                Image* valueBg = UtilitUI::AddImage(valueBtnGO, inputBgTexture,
                    Vector2(basePosition.x - 140, paramY), Vector2(200, 25),
                    baseAnchorMin, baseAnchorMax,
                    color6);
                valueBg->SetLayer(UtilitUI::Layers::InspectorButton);
                canvas->AddUIElement(valueBg);
                
                Text* valueText = UtilitUI::AddTextWithAnchors(valueBtnGO, inspectorFont, paramValue,
                    Vector2(basePosition.x - 140, paramY + 3), Vector2(190, 20),
                    baseAnchorMin, baseAnchorMax,
                    color4);
                valueText->SetLayer(UtilitUI::Layers::InspectorText);
                valueText->SetVisible(false);
                canvas->AddUIElement(valueText);
                
                Button* valueButton = valueBtnGO->AddComponent<Button>();
                valueButton->SetUIElement(valueBg);
                valueButton->SetCanvas(canvas);
            }
            
            yOffset -= 100;
        }
    }

    void UpdateComponentPositions(int totalComponents) {
        float yOffset = basePosition.y + 400 + yScroll;
        int index = 0;
        
        for (auto* obj : componentObjects) {
            if (!obj) continue;
            
            float yPos = yOffset - (index * 100);
            
            const std::vector<Component*>& components = obj->GetComponents();
            for (Component* comp : components) {
                if (Image* img = dynamic_cast<Image*>(comp)) {
                    Vector2 currentPos = img->rectTransform->GetAnchoredPosition();
                    img->rectTransform->SetAnchoredPosition(Vector2(currentPos.x, yPos));
                }
            }
            index++;
        }
    }

    void DestroyUIObject(GameObject* obj) {
        if (!obj || !canvas) return;
        
        const std::vector<Component*>& components = obj->GetComponents();
        for (Component* comp : components) {
            if (Image* img = dynamic_cast<Image*>(comp)) {
                canvas->RemoveUIElement(img);
            }
            if (Text* txt = dynamic_cast<Text*>(comp)) {
                canvas->RemoveUIElement(txt);
            }
            if (Button* btn = dynamic_cast<Button*>(comp)) {
                btn->ClearAllSubscriptions();
            }
        }
        
        scene->RemoveGameObject(obj);
    }

    void HandleScroll() {
        if (!isLoaded) return;
        
        double newScroll = InputSystem::GetInstance().GetMouseScrollY();
        float deltaScroll = static_cast<float>(lastScroll - newScroll);
        lastScroll = newScroll;
        
        yScroll += deltaScroll * speedScroll;
        yScroll = std::clamp(yScroll, minScroll, maxScroll);
    }

    void CalculateScrollBounds(int totalComponents) {
        float totalHeight = totalComponents * 100;
        float visibleHeight = 600;
        maxScroll = 0.0f;
        minScroll = -(totalHeight - visibleHeight);
        if (minScroll > 0) minScroll = 0;
        yScroll = std::clamp(yScroll, minScroll, maxScroll);
    }

    void SetOnAddComponentCallback(std::function<void()> callback) {
        onAddComponentCallback = callback;
    }
};