#include "Editor.h"

#include "RegisterAllComponents.h"

int main(){
    SpaceRegisterComponents::RegisterAllComponents();

    Editor* editor = new Editor();

    bool isInitialize = editor->Start();
    if(!isInitialize) return -1;

    Scene* gameScene = editor->GetCurrentGameScene();
    
    GameObject* testObject1 = gameScene->CreateGameObject();
    testObject1->SetName("TestPlayer");
    
    GameObject* testObject2 = gameScene->CreateGameObject();
    testObject2->SetName("TestEnemy");
    
    GameObject* testObject3 = gameScene->CreateGameObject();
    testObject3->SetName("TestWall");

    while(editor->IsRunEditor()){
        editor->Update();
    }

    editor->Destroy();

    return 0;
}