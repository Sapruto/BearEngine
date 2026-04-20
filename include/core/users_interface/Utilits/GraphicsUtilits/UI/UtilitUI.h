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

    inline Text* AddTextWithAnchors(GameObject* gameObject, Font* font, const std::string& text, 
                                    Vector2 position, Vector2 size, 
                                    Vector2 anchorMin, Vector2 anchorMax,
                                    glm::vec4 color) {
        Text* txt = gameObject->AddComponent<Text>(*font, text);
        txt->rectTransform->SetAnchorMin(anchorMin);
        txt->rectTransform->SetAnchorMax(anchorMax);
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
}