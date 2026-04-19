#pragma once

#include "Window.h"
#include "Camera3D.h"
#include "UIRendering.h"
#include "GraphicsManager.h"
#include "ResourceManager.h"

namespace UtilitGraphic {
    inline bool InitGraphics(GraphicsManager* mgr, ResourceManager& resources) {
        float clearColor[4] = {0.1f, 0.1f, 0.15f, 1.0f};
        Window* window = new Window(1600, 900, clearColor, "BEAR ENGINE");
        if (!window->Initialize()) {
            delete window;
            return false;
        }
        mgr->SetWindow(window);

        Camera3D* camera = new Camera3D(Vector3(0, 10, 20));
        mgr->SetCamera(camera);

        return true;
    }
}