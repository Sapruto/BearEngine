#pragma once

#include "Camera3D.h"
#include "ModelRenderer.h"

#include <memory>
#include <vector>

class SceneFileController;
class GraphicsManager;
class GameObject;
class Component;

class SceneView {
private:
    SceneFileController* controller{nullptr};
    GraphicsManager* graphicsManager{nullptr};
    
    std::vector<Component*> editComponents;
    
    Camera3D editorCamera;
    ModelRenderer* modelRenderer{nullptr};
    
    int viewportX{25};
    int viewportY{250};
    int viewportWidth{800};
    int viewportHeight{600};

public:
    SceneView(SceneFileController* controller, GraphicsManager* gfxMgr);
    
    void Start();
    void Update();
    void Render();
    void ResizeViewport(int x, int y, int width, int height);
    
    void AddEditComponent(Component* comp) { editComponents.push_back(comp); }
    void ClearEditComponents() { editComponents.clear(); }
};