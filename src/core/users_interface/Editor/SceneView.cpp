#include "SceneView.h"

#include "Editor.h"
#include "SceneCreator.h"
#include "GraphicsManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void SceneView::Start() {
    std::unique_ptr<Scene> loadedScene = creator->GetScene(path);
    if (!loadedScene) return;
    
    editor->SetGameScene(std::move(loadedScene));
    
    Scene* editScene = editor->GetEditorScene();
    Scene* gameScene = editor->GetCurrentGameScene();
    
    for (auto* gameObject : gameScene->GetGameObjects()){
        GameObject* editObject = editScene->CreateGameObject();
        
        std::vector<Component*> gameObjectComponents = gameObject->GetComponents();
        
        for (Component* comp : gameObjectComponents) {
            gameObject->RemoveComponent(comp);
            editObject->AddComponent(comp);
        }
        
        for (Component* comp : editComponents) {
            editObject->AddComponent(comp);
        }
    }
    
    editorCamera.position = Vector3(0, 5, 10);
    editorCamera.updateVectors();
    
    modelRenderer = editor->GetGraphicsManager()->GetRenderer<ModelRenderer>();
    if (modelRenderer) {
        editor->GetGraphicsManager()->SetCamera(&editorCamera);
    }
}

void SceneView::Update() {
    Render();
}

void SceneView::Render() {
    if (!modelRenderer) return;
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    glViewport(50, 50, 800, 600);
    glEnable(GL_DEPTH_TEST);
    
    modelRenderer->Update();
    
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}