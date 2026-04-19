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

namespace UtilitUI {
    inline static std::string resource_path = "Assets/Base/";

    inline Image* AddImage(
        GameObject* gameObject,
        Texture* uiTexture,
        Vector2 position,
        Vector2 deltaSize,
        Vector2 anchorMin,
        Vector2 anchorMax,
        glm::vec4 color
    ) {
        Image* image = gameObject->AddComponent<Image>();
        image->SetTexture(uiTexture);
        image->SetLayer(0);
        image->rectTransform->SetAnchorMin(anchorMin);
        image->rectTransform->SetAnchorMax(anchorMax);
        image->rectTransform->SetAnchoredPosition(position);
        image->rectTransform->SetSizeDelta(deltaSize);
        image->SetColor(color);
        return image;
    }

    inline Text* AddText(GameObject* gameObject, Font* font, const std::string& text, Vector2 position, Vector2 size, glm::vec4 color) {
        Text* txt = gameObject->AddComponent<Text>(*font, text);
        txt->rectTransform->SetAnchorMin(Vector2(0, 0));
        txt->rectTransform->SetAnchorMax(Vector2(0, 0));
        txt->rectTransform->SetAnchoredPosition(position);
        txt->rectTransform->SetSizeDelta(size);
        txt->SetColor(color);
        return txt;
    }

    class EditorUI {
    private:
        Font* baseFont{nullptr};
        Texture* baseTextureButton{nullptr};
        UIRendering* uiRenderer{nullptr};
        ResourceManager* resources{nullptr};

        Scene* scene{nullptr};

    public:
        EditorUI(UIRendering* renderer, ResourceManager* resMgr, Scene* scn) 
            : uiRenderer(renderer), resources(resMgr), scene(scn) {}

        void LoadResources() {
            if (!resources) return;
            
            resources->LoadResource(resource_path + "fonts/arial.ttf", ResourceType::Font);
            resources->LoadResource(resource_path + "ui/button.png", ResourceType::Texture);

            baseFont = resources->GetResourceAs<Font>(resource_path + "fonts/arial.ttf");
            baseTextureButton = resources->GetResourceAs<Texture>(resource_path + "ui/button.png");
        }

        void CreateUI(Canvas* canvas) {
            GameObject* bgGO = scene->CreateGameObject();
            bgGO->SetName("EditorBackground");
            Image* bg = AddImage(bgGO, baseTextureButton,
                Vector2(0, 0), Vector2(1920, 1080),
                Vector2(0, 0), Vector2(1, 1),
                glm::vec4(0.1f, 0.1f, 0.15f, 1.0f));
            canvas->AddUIElement(bg);

            GameObject* logoGO = scene->CreateGameObject();
            logoGO->SetName("Logo");
            Image* logo = AddImage(logoGO, baseTextureButton,
                Vector2(20, 20), Vector2(128, 128),
                Vector2(0, 0), Vector2(0, 0),
                glm::vec4(1.0f));
            canvas->AddUIElement(logo);
        }

        UIRendering* GetUIRenderer() { return uiRenderer; }

        Font* GetFont() { return baseFont; }
        Texture* GetButtonTexture() { return baseTextureButton; }
    };

    class HierarchyViewerUI {
    private:
        Font* objectFont{nullptr};
        Texture* objectTextureButton{nullptr};

        Canvas* canvas{nullptr};
        ResourceManager* resources{nullptr};

        Vector2 anchoredMin = Vector2(0.8f, 0.0f);
        Vector2 anchoredMax = Vector2(0.8f, 0.0f);
        float x = -250.0f;
        float yOffset = 50.0f;
        float ySpacing = 35.0f;

    public:
        HierarchyViewerUI(Canvas* cv, ResourceManager* resMgr) 
            : canvas(cv), resources(resMgr) {}

        void LoadResources() {
            if (!resources) return;
            
            resources->LoadResource(resource_path + "fonts/arial.ttf", ResourceType::Font);
            resources->LoadResource(resource_path + "ui/button.png", ResourceType::Texture);

            objectFont = resources->GetResourceAs<Font>(resource_path + "fonts/arial.ttf");
            objectTextureButton = resources->GetResourceAs<Texture>(resource_path + "ui/button.png");
        }

        Canvas* GetCanvas() { return canvas; }

        GameObject* CreateUIObject(GameObject* gameObject, int index) {
            if(!canvas) return nullptr;

            float y = yOffset + index * ySpacing;
            
            GameObject* obj = new GameObject(gameObject->GetName());
            
            Tag* tag = obj->AddComponent<Tag>(AllTags::HierarchyViewerUI);
            
            Image* bg = obj->AddComponent<Image>();
            bg->SetTexture(objectTextureButton);
            bg->SetColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
            bg->rectTransform->SetAnchorMin(anchoredMin);
            bg->rectTransform->SetAnchorMax(anchoredMax);
            bg->rectTransform->SetAnchoredPosition(Vector2(x, y));
            bg->rectTransform->SetSizeDelta(Vector2(250, 30));
            canvas->AddUIElement(bg);
            bg->SetCanvas(canvas);
            
            Button* btn = obj->GetComponentOfType<Button>();
            if(!btn) btn = obj->AddComponent<Button>();
            btn->SetUIElement(bg);
            btn->SetCanvas(canvas);
            btn->Start();
            
            Text* label = nullptr;
            if (objectFont) {
                label = obj->AddComponent<Text>(*objectFont, gameObject->GetName());
                label->SetColor(glm::vec4(1.0f));
                label->rectTransform->SetAnchorMin(anchoredMin);
                label->rectTransform->SetAnchorMax(anchoredMax);
                label->rectTransform->SetAnchoredPosition(Vector2(x, y));
                label->rectTransform->SetSizeDelta(Vector2(230, 20));
                canvas->AddUIElement(label);
            }
            
            return obj;
        }
        
        void UpdateUIObjectPosition(GameObject* obj, int index) {
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
        
        void UpdateUIObjectText(GameObject* obj, const std::string& name) {
            Text* label = obj->GetComponentOfType<Text>();
            if (label) {
                label->SetText(name);
            }
        }
        
        void DestroyUIObject(GameObject* obj) {
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
            
            delete obj;
        }
    };

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

    public:
        InspectorUI(Canvas* cv, ResourceManager* resMgr, Scene* scn)
            : canvas(cv), resources(resMgr), scene(scn) {}

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

        void ShowObjectComponents(GameObject* currentGameObject, 
                                std::unordered_map<GameObject*, DrawComponentData>& inspectorObjects) {
            if (!currentGameObject || !scene || !isLoaded) return;
            
            GameObject* headerGO = scene->CreateGameObject();
            Image* headerBg = UtilitUI::AddImage(headerGO, buttonTexture,
                Vector2(-300, 500), Vector2(400, 40),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.3f, 0.3f, 0.35f, 1.0f));
            
            Text* headerText = UtilitUI::AddText(headerGO, inspectorFont, 
                "Inspector: " + currentGameObject->GetName(),
                Vector2(-300, 505), Vector2(380, 30),
                glm::vec4(1.0f));
            
            GameObject* addBtnGO = scene->CreateGameObject();
            Image* addBtnBg = UtilitUI::AddImage(addBtnGO, buttonTexture,
                Vector2(-300, 450), Vector2(150, 30),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.2f, 0.6f, 0.2f, 1.0f));
            
            Text* addBtnText = UtilitUI::AddText(addBtnGO, inspectorFont, "+ Add Component",
                Vector2(-300, 455), Vector2(140, 25),
                glm::vec4(1.0f));
            
            Button* addButton = addBtnGO->AddComponent<Button>();
            addButton->SetUIElement(addBtnBg);
            
            float yOffset = 400;
            for (auto& [obj, data] : inspectorObjects) {
                if (obj == activeInputObject) continue;
                
                GameObject* compGO = scene->CreateGameObject();
                Image* compBg = UtilitUI::AddImage(compGO, componentBgTexture,
                    Vector2(-300, yOffset), Vector2(400, 80),
                    Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                    glm::vec4(0.25f, 0.25f, 0.3f, 1.0f));
                
                Text* compName = UtilitUI::AddText(compGO, inspectorFont, data.componentName,
                    Vector2(-280, yOffset + 25), Vector2(200, 25),
                    glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
                
                GameObject* removeBtnGO = scene->CreateGameObject();
                Image* removeBg = UtilitUI::AddImage(removeBtnGO, buttonTexture,
                    Vector2(50, yOffset + 25), Vector2(60, 25),
                    Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                    glm::vec4(0.7f, 0.2f, 0.2f, 1.0f));
                
                Text* removeText = UtilitUI::AddText(removeBtnGO, inspectorFont, "X",
                    Vector2(50, yOffset + 28), Vector2(60, 20),
                    glm::vec4(1.0f));
                
                Button* removeButton = removeBtnGO->AddComponent<Button>();
                removeButton->SetUIElement(removeBg);
                
                float paramY = yOffset - 10;
                for (auto& [paramName, paramValue] : data.nameToParams) {
                    paramY -= 30;
                    
                    Text* paramText = UtilitUI::AddText(compGO, inspectorFont, paramName + ":",
                        Vector2(-280, paramY), Vector2(120, 25),
                        glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
                    
                    GameObject* valueBtnGO = scene->CreateGameObject();
                    Image* valueBg = UtilitUI::AddImage(valueBtnGO, inputBgTexture,
                        Vector2(-140, paramY), Vector2(200, 25),
                        Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                        glm::vec4(0.15f, 0.15f, 0.2f, 1.0f));
                    
                    Text* valueText = UtilitUI::AddText(valueBtnGO, inspectorFont, paramValue,
                        Vector2(-140, paramY + 3), Vector2(190, 20),
                        glm::vec4(1.0f));
                    
                    Button* valueButton = valueBtnGO->AddComponent<Button>();
                    valueButton->SetUIElement(valueBg);
                }
                
                yOffset -= 100;
            }
        }
        
        void ShowAllComponents(GameObject* currentGameObject, std::vector<std::string>& componentNames) {
            if (!currentGameObject || !scene || !isLoaded) return;
            
            float yOffset = 300;
            float startX = 100;
            
            for (size_t i = 0; i < componentNames.size(); i++) {
                GameObject* compBtnGO = scene->CreateGameObject();
                
                Image* btnBg = UtilitUI::AddImage(compBtnGO, buttonTexture,
                    Vector2(startX, yOffset - (i * 35)), Vector2(200, 30),
                    Vector2(0, 0), Vector2(0, 0),
                    glm::vec4(0.3f, 0.3f, 0.4f, 1.0f));
                
                Text* btnText = UtilitUI::AddText(compBtnGO, inspectorFont, componentNames[i],
                    Vector2(startX + 10, yOffset - (i * 35) + 5), Vector2(180, 25),
                    glm::vec4(1.0f));
                
                Button* button = compBtnGO->AddComponent<Button>();
                button->SetUIElement(btnBg);
            }
        }

        void ShowComponentSettings(GameObject* currentGameObject) {
            if (!currentGameObject || !scene || !isLoaded) return;
            
            GameObject* settingsGO = scene->CreateGameObject();
            Image* settingsBg = UtilitUI::AddImage(settingsGO, buttonTexture,
                Vector2(0, 0), Vector2(400, 300),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.2f, 0.2f, 0.25f, 0.95f));
            
            Text* titleText = UtilitUI::AddText(settingsGO, inspectorFont, "Component Settings",
                Vector2(-150, 120), Vector2(300, 30),
                glm::vec4(1.0f));
            
            GameObject* closeBtnGO = scene->CreateGameObject();
            Image* closeBg = UtilitUI::AddImage(closeBtnGO, buttonTexture,
                Vector2(170, 130), Vector2(40, 30),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.7f, 0.2f, 0.2f, 1.0f));
            
            Text* closeText = UtilitUI::AddText(closeBtnGO, inspectorFont, "X",
                Vector2(170, 133), Vector2(40, 25),
                glm::vec4(1.0f));
            
            Button* closeButton = closeBtnGO->AddComponent<Button>();
            closeButton->SetUIElement(closeBg);
        }
    };
}