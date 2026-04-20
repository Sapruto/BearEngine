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

#include "GraphicsUtilits/UI/UtilitUI.h"

#include "Inspector.h"

#include "UILayers.h"

#include "InputSystem.h"

#include <algorithm>

namespace UtilitUI {
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

        int currentIndex{0};

        bool isLoaded{false};

        float minScroll;
        float maxScroll;
        float yScroll;
        float speedScroll;
        float lastScroll;

        std::vector<GameObject*> helperObjects;

    public:
        InspectorUI(Canvas* cv, ResourceManager* resMgr, Scene* scn)
            : canvas(cv), resources(resMgr), scene(scn), minScroll(0), maxScroll(0), yScroll(0), speedScroll(20), lastScroll(0) {}

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
            currentIndex = 0;
        }

        void ClearHelperObjects(){
            if(!scene) return;

            for(auto* object : helperObjects){
                if(object){
                    DestroyUIObject(object);
                }
            }
            helperObjects.clear();
        }

        void CreateObjectComponents(GameObject* currentGameObject, 
                                    std::unordered_map<GameObject*, DrawComponentData>& inspectorObjects) {
            if (!currentGameObject || !scene || !isLoaded) return;
            
            GameObject* headerGO = scene->CreateGameObject();
            helperObjects.push_back(headerGO);
            
            Image* headerBg = UtilitUI::AddImage(headerGO, buttonTexture,
                Vector2(-300, 500), Vector2(400, 40),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.3f, 0.3f, 0.35f, 1.0f));
            headerBg->SetLayer(Layers::InspectorButton);
            canvas->AddUIElement(headerBg);
            
            Text* headerText = UtilitUI::AddTextWithAnchors(headerGO, inspectorFont, 
                "Inspector: " + currentGameObject->GetName(),
                Vector2(-300, 505), Vector2(380, 30),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(1.0f));
            headerText->SetLayer(Layers::InspectorText);
            canvas->AddUIElement(headerText);
            
            GameObject* addBtnGO = scene->CreateGameObject();
            helperObjects.push_back(addBtnGO);
            
            Image* addBtnBg = UtilitUI::AddImage(addBtnGO, buttonTexture,
                Vector2(-300, 450), Vector2(150, 30),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.2f, 0.6f, 0.2f, 1.0f));
            addBtnBg->SetLayer(Layers::InspectorButton);
            canvas->AddUIElement(addBtnBg);
            
            Text* addBtnText = UtilitUI::AddTextWithAnchors(addBtnGO, inspectorFont, "+ Add Component",
                Vector2(-300, 455), Vector2(140, 25),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(1.0f));
            addBtnText->SetLayer(Layers::InspectorText);
            canvas->AddUIElement(addBtnText);
            
            Button* addButton = addBtnGO->AddComponent<Button>();
            addButton->SetUIElement(addBtnBg);
            addButton->SetCanvas(canvas);
            
            float yOffset = 400;
            for (auto& [obj, data] : inspectorObjects) {
                if (obj == activeInputObject) continue;
                
                Image* compBg = UtilitUI::AddImage(obj, componentBgTexture,
                    Vector2(-300, yOffset), Vector2(400, 80),
                    Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                    glm::vec4(0.25f, 0.25f, 0.3f, 1.0f));
                compBg->SetLayer(Layers::InspectorButton);
                canvas->AddUIElement(compBg);
                
                Text* compName = UtilitUI::AddTextWithAnchors(obj, inspectorFont, data.componentName,
                    Vector2(-280, yOffset + 25), Vector2(200, 25),
                    Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                    glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
                compName->SetLayer(Layers::InspectorText);
                canvas->AddUIElement(compName);
                
                GameObject* removeBtnGO = scene->CreateGameObject();
                helperObjects.push_back(removeBtnGO);
                
                Image* removeBg = UtilitUI::AddImage(removeBtnGO, buttonTexture,
                    Vector2(50, yOffset + 25), Vector2(60, 25),
                    Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                    glm::vec4(0.7f, 0.2f, 0.2f, 1.0f));
                removeBg->SetLayer(Layers::InspectorButton);
                canvas->AddUIElement(removeBg);
                
                Text* removeText = UtilitUI::AddTextWithAnchors(removeBtnGO, inspectorFont, "X",
                    Vector2(50, yOffset + 28), Vector2(60, 20),
                    Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                    glm::vec4(1.0f));
                removeText->SetLayer(Layers::InspectorText);
                canvas->AddUIElement(removeText);
                
                Button* removeButton = removeBtnGO->AddComponent<Button>();
                removeButton->SetUIElement(removeBg);
                removeButton->SetCanvas(canvas);
                
                float paramY = yOffset - 10;
                for (auto& [paramName, paramValue] : data.nameToParams) {
                    paramY -= 30;
                    
                    Text* paramText = UtilitUI::AddTextWithAnchors(obj, inspectorFont, paramName + ":",
                        Vector2(-280, paramY), Vector2(120, 25),
                        Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                        glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
                    paramText->SetLayer(Layers::InspectorText);
                    canvas->AddUIElement(paramText);
                    
                    GameObject* valueBtnGO = scene->CreateGameObject();
                    helperObjects.push_back(valueBtnGO);
                    
                    Image* valueBg = UtilitUI::AddImage(valueBtnGO, inputBgTexture,
                        Vector2(-140, paramY), Vector2(200, 25),
                        Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                        glm::vec4(0.15f, 0.15f, 0.2f, 1.0f));
                    valueBg->SetLayer(Layers::InspectorButton);
                    canvas->AddUIElement(valueBg);
                    
                    Text* valueText = UtilitUI::AddTextWithAnchors(valueBtnGO, inspectorFont, paramValue,
                        Vector2(-140, paramY + 3), Vector2(190, 20),
                        Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                        glm::vec4(1.0f));
                    valueText->SetLayer(Layers::InspectorText);
                    canvas->AddUIElement(valueText);
                    
                    Button* valueButton = valueBtnGO->AddComponent<Button>();
                    valueButton->SetUIElement(valueBg);
                    valueButton->SetCanvas(canvas);
                }
                
                yOffset -= 100;
            }
        }

        void CreateAllComponents(GameObject* currentGameObject, std::vector<std::string>& componentNames) {
            if (!currentGameObject || !scene || !isLoaded) return;
            
            float yOffset = 300;
            float startX = 100;
            
            for (size_t i = 0; i < componentNames.size(); i++) {
                GameObject* compBtnGO = scene->CreateGameObject();
                helperObjects.push_back(compBtnGO);
                
                Image* btnBg = UtilitUI::AddImage(compBtnGO, buttonTexture,
                    Vector2(startX, yOffset - (i * 35)), Vector2(200, 30),
                    Vector2(0, 0), Vector2(0, 0),
                    glm::vec4(0.3f, 0.3f, 0.4f, 1.0f));
                btnBg->SetLayer(Layers::InspectorButton);
                canvas->AddUIElement(btnBg);
                
                Text* btnText = UtilitUI::AddTextWithAnchors(compBtnGO, inspectorFont, componentNames[i],
                    Vector2(startX + 10, yOffset - (i * 35) + 5), Vector2(180, 25),
                    Vector2(0, 0), Vector2(0, 0),
                    glm::vec4(1.0f));
                btnText->SetLayer(Layers::InspectorText);
                canvas->AddUIElement(btnText);
                
                Button* button = compBtnGO->AddComponent<Button>();
                button->SetUIElement(btnBg);
                button->SetCanvas(canvas);
            }
        }

        void CreateComponentSettings(GameObject* currentGameObject) {
            if (!currentGameObject || !scene || !isLoaded) return;
            
            GameObject* settingsGO = scene->CreateGameObject();
            helperObjects.push_back(settingsGO);
            
            Image* settingsBg = UtilitUI::AddImage(settingsGO, buttonTexture,
                Vector2(0, 0), Vector2(400, 300),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.2f, 0.2f, 0.25f, 0.95f));
            settingsBg->SetLayer(Layers::InspectorButton);
            canvas->AddUIElement(settingsBg);
            
            Text* titleText = UtilitUI::AddTextWithAnchors(settingsGO, inspectorFont, "Component Settings",
                Vector2(-150, 120), Vector2(300, 30),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(1.0f));
            titleText->SetLayer(Layers::InspectorText);
            canvas->AddUIElement(titleText);
            
            GameObject* closeBtnGO = scene->CreateGameObject();
            helperObjects.push_back(closeBtnGO);
            
            Image* closeBg = UtilitUI::AddImage(closeBtnGO, buttonTexture,
                Vector2(170, 130), Vector2(40, 30),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.7f, 0.2f, 0.2f, 1.0f));
            closeBg->SetLayer(Layers::InspectorButton);
            canvas->AddUIElement(closeBg);
            
            Text* closeText = UtilitUI::AddTextWithAnchors(closeBtnGO, inspectorFont, "X",
                Vector2(170, 133), Vector2(40, 25),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(1.0f));
            closeText->SetLayer(Layers::InspectorText);
            canvas->AddUIElement(closeText);
            
            Button* closeButton = closeBtnGO->AddComponent<Button>();
            closeButton->SetUIElement(closeBg);
            closeButton->SetCanvas(canvas);
        }

        void UpdateUIObjectPosition(GameObject* obj, int index) {
            if (!obj || !canvas) return;
            
            float yOffset = 400 - (index * 100) + yScroll;
            
            const std::vector<Component*>& components = obj->GetComponents();
            for (Component* comp : components) {
                if (Image* img = dynamic_cast<Image*>(comp)) {
                    Vector2 currentPos = img->rectTransform->GetAnchoredPosition();
                    img->rectTransform->SetAnchoredPosition(Vector2(currentPos.x, yOffset));
                }
                if (Text* txt = dynamic_cast<Text*>(comp)) {
                    Vector2 currentPos = txt->rectTransform->GetAnchoredPosition();
                    txt->rectTransform->SetAnchoredPosition(Vector2(currentPos.x, yOffset));
                }
            }
        }

        void UpdateUIObjectText(GameObject* obj, const std::string& name) {
            if (!obj || !canvas) return;
            
            const std::vector<Component*>& components = obj->GetComponents();
            for (Component* comp : components) {
                if (Text* txt = dynamic_cast<Text*>(comp)) {
                    txt->SetText(name);
                }
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
    };
}