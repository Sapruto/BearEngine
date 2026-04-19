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
        Font* baseFont = nullptr;
        Texture* baseTextureButton = nullptr;
        UIRendering* uiRenderer = nullptr;
        ResourceManager* resources = nullptr;

        Scene* scene = nullptr;

    public:
        void Init(UIRendering* renderer, ResourceManager* resMgr, Scene* scene) {
            uiRenderer = renderer;
            resources = resMgr;
            this->scene = scene;
        }

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

            GameObject* btnGO = scene->CreateGameObject();
            btnGO->SetName("TestButton");
            Button* btn = btnGO->AddComponent<Button>();
            Image* imageBtn = AddImage(btnGO, baseTextureButton,
                Vector2(800, 450), Vector2(200, 80),
                Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f),
                glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
            canvas->AddUIElement(imageBtn);
        }

        UIRendering* GetUIRenderer() { return uiRenderer; }

        Font* GetFont() { return baseFont; }
        Texture* GetButtonTexture() { return baseTextureButton; }
    };

    class HierarchyViewerUI {
    private:
        Canvas* canvas;
        Font* font;
        Texture* buttonTexture;
        float yOffset = 50.0f;
        float ySpacing = 35.0f;
        int currentIndex = 0;

    public:
        HierarchyViewerUI(Canvas* cv, Font* f, Texture* tex) 
            : canvas(cv), font(f), buttonTexture(tex) {}

        void BeginDraw() {
            currentIndex = 0;
        }

        GameObject* DrawObject(GameObject* gameObject) {
            GameObject* obj = new GameObject(gameObject->GetName());

            float y = yOffset + currentIndex * ySpacing;
            currentIndex++;
            
            glm::vec4 bgColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        
            Image* bg = obj->GetComponentOfType<Image>();
            if (!bg) {
                bg = obj->AddComponent<Image>();
            }
            bg->SetTexture(buttonTexture);
            bg->SetColor(bgColor);
            bg->rectTransform->SetAnchorMin(Vector2(0, 0));
            bg->rectTransform->SetAnchorMax(Vector2(0, 0));
            bg->rectTransform->SetAnchoredPosition(Vector2(10, y));
            bg->rectTransform->SetSizeDelta(Vector2(250, 30));
            canvas->AddUIElement(bg);
            
            Text* label = obj->GetComponentOfType<Text>();
            if (!label) {
                label = obj->AddComponent<Text>(*font, obj->GetName());
            } else {
                label->SetText(obj->GetName());
            }
            label->SetColor(glm::vec4(1.0f));
            label->rectTransform->SetAnchorMin(Vector2(0, 0));
            label->rectTransform->SetAnchorMax(Vector2(0, 0));
            label->rectTransform->SetAnchoredPosition(Vector2(20, y + 5));
            label->rectTransform->SetSizeDelta(Vector2(230, 20));
            canvas->AddUIElement(label);

            return obj;
        }
    };
}