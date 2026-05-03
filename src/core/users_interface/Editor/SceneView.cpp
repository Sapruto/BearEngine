#include "SceneView.h"
#include "SceneFileController.h"
#include "GraphicsManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "Tag.h"
#include "EditorTags.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

SceneView::SceneView(SceneFileController* controller, GraphicsManager* gfxMgr) 
    : controller(controller), graphicsManager(gfxMgr) {}

void SceneView::Start() {
    if (!controller) return;
    
    Scene* gameScene = controller->GetCurrentGameScene();
    Scene* editScene = controller->GetEditorScene();
    
    if (!gameScene || !editScene) return;
    
    for (auto* gameObject : gameScene->GetGameObjects()) {
        GameObject* editObject = editScene->CreateGameObject();
        editObject->SetName(gameObject->GetName() + "_avatar");
        
        for (auto* comp : gameObject->GetComponents()) {
            editObject->AddComponent(comp);
        }
        
        for (auto* comp : editComponents) {
            editObject->AddComponent(comp);
        }
        
        editObject->AddComponent<Tag>(AllTags::SceneViewerObjects);
        
        controller->RegisterAvatar(gameObject, editObject);
    }
    
    modelRenderer = graphicsManager->GetRenderer<ModelRenderer>();
    if (!modelRenderer) {
        modelRenderer = new ModelRenderer();
    }
}

void SceneView::Update() {

}

void SceneView::Render() {
    if (!graphicsManager || !graphicsManager->GetCamera()) return;
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewportX, viewportY, viewportWidth, viewportHeight);
    glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    //if (modelRenderer) modelRenderer->Update();
    
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void SceneView::ResizeViewport(int x, int y, int width, int height) {
    viewportX = x;
    viewportY = y;
    viewportWidth = width;
    viewportHeight = height;
}