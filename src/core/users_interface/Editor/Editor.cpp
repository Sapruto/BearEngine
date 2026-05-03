#include "Editor.h"

#include "Time.h"

Editor::Editor()
    : input(InputSystem::GetInstance())
    , sceneController(this, "Assets/scenes/scene67.scene", input)
{
}

bool Editor::Start() {
    if (!UtilitGraphic::InitGraphics(&graphicsManager, resources)) return false;
    Font::InitFreeType();
    input.Initialize(graphicsManager.GetWindow()->GetWindow());

    sceneController.Start();
    if (!toolsController.Initialize(this, &graphicsManager, &resources, sceneController.GetEditorScene())) return false;
    
    return true;
}

void Editor::Update() {
    Time::Tick();
    input.Update();
    
    Window* window = graphicsManager.GetWindow();
    if (!window) return;
    
    window->Clear();

    sceneController.Update();
    
    toolsController.Update();
    graphicsManager.Update();
    
    window->SwapBuffers();
}

Scene* Editor::GetCurrentGameScene() { return sceneController.GetCurrentGameScene(); }