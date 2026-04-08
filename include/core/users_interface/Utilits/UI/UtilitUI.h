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

        GameObject button
    }

    class SceneViewUI{
    private:
        Font* baseFont;
        Texture* baseTextureButton;

    public:
        void LoatResources(){
            resources.LoadResource(resource_path + "fonts/arial.ttf");
            resources.LoadResource(resource_path + "ui/button.png");

            baseFont = GetResourceAs<Font>("fonts/arial.ttf");
            baseTextureButton = GetResourceAs<Texture>("ui/button.png");
        }
    };
}