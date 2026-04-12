#pragma once

#include "Scene.h"
#include "GameObject.h"
#include "GraphicsManager.h"

#include "Texture.h"

#include "Font.h"
#include "Button.h"
#include "Image.h"

namespace UtilitUI{
    static std::string resource_path = "Assets/Base"

    void InitGraphics(Scene* scene, GraphicsManager* mgr, ResourceManager& resources){
        float clearColor[4] = {0.1f, 0.1f, 0.15f, 1.0f};
        Window* window = new Window(1600, 900, clearColor, "BEAR ENGINE - 3D Platformer");
        if (!window->Initialize()) return -1;
        mgr->SetWindow(window);

        Camera3D* camera = new Camera3D(Vector3(0, 10, 20));
        mgr->SetCamera(camera);

        UIRendering* uiRenderer = graphics.AddRender<UIRendering>();
        InitUI(uiRenderer, resources)
    }

    void InitUI(UIRendering* uiRenderer, ResourceManager& resources){
        resources.LoadResource(resource_path + "fonts/arial.ttf");
        resources.LoadResource(resource_path + "ui/button.png");

        Font* font = GetResourceAs<Font>("fonts/arial.ttf");
        
        Texture* buttonTexture = GetResourceAs<Texture>("ui/button.png");
    }

    Image* AddImage(GameObject* gameObject, Texture* uiTexture, Vector2 position, Vector2 deltaSize,Vector2 anchorMin, Vector2 anchorMax, glm::vec4 color){
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

    class SceneViewGraphic{
    private:
        Font* baseFont;
        Texture* baseTextureButton;

        UIRendering* uiRenderer;

    public:
        void LoatResources(){
            uiRenderer = new UIRendering();

            resources.LoadResource(resource_path + "fonts/arial.ttf");
            resources.LoadResource(resource_path + "ui/button.png");

            baseFont = GetResourceAs<Font>("fonts/arial.ttf");
            baseTextureButton = GetResourceAs<Texture>("ui/button.png");
        }

        void CreateUI(){
            GameObject* bggameObject = new GameObject();
            Image* backgroundImage = AddImage(bggameObject);

            GameObject* logoGO = new GameObject();
            Image* logo = logoGO->AddComponent<Image>();
            logo->SetTexture(uiTexture);
            logo->SetLayer(1);
            logo->rectTransform->SetAnchorMin(Vector2(0, 0));
            logo->rectTransform->SetAnchorMax(Vector2(0, 0));
            logo->rectTransform->SetAnchoredPosition(Vector2(100, 100));
            logo->rectTransform->SetSizeDelta(Vector2(256, 256));
            logo->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            uiCanvas->AddUIElement(logo);

            GameObject* buttonGO = new GameObject();
            Image* button = buttonGO->AddComponent<Image>();
            button->SetTexture(uiTexture);
            button->SetLayer(2);
            button->rectTransform->SetAnchorMin(Vector2(0, 0));
            button->rectTransform->SetAnchorMax(Vector2(0, 0));
            button->rectTransform->SetAnchoredPosition(Vector2(800, 500));
            button->rectTransform->SetSizeDelta(Vector2(200, 80));
            button->SetColor(glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
            uiCanvas->AddUIElement(button);
        }

        UIRendering* GetUIRenderer(){

        }
    };
}