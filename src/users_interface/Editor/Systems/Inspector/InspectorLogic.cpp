#include "InspectorLogic.h"

#include "InspectorUI.h"

#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "SerializeField.h"

void InspectorLogic::ClearInspector(){
    ui->ClearAll();
    inspectorObjects.clear();
}

void InspectorLogic::Start(){
    if (!currentGameObject) return;

    componentNames = ComponentRegistry::GetAllNames();

    std::vector<Component*> components = currentGameObject->GetComponents();

    for(auto* comp : components){
        std::string name = ComponentRegistry::GetNameByComponent(comp);
        DrawComponentData data = DrawComponentData(name);

        if (auto* serializable = dynamic_cast<ISerializable*>(comp)) {
            for (auto* field : serializable->GetSerializedFields()) {
                std::string fieldName = field->GetName();
                std::string fieldValue = field->ToString();
                data.nameToParams[fieldName] = fieldValue;
            }
        }
        
        GameObject* object = scene->CreateGameObject();
        inspectorObjects[object] = data;
    }

    ui->CalculateScrollBounds(components.size());
    ui->CreateHeader(currentGameObject);
    
    ui->CreateObjectComponents(inspectorObjects,
        [this](GameObject* componentUI, const std::string& compName) {
            auto componentsList = currentGameObject->GetComponents();
            for (auto* comp : componentsList) {
                if (ComponentRegistry::GetNameByComponent(comp) == compName) {
                    currentGameObject->RemoveComponent(comp);
                    ClearInspector();
                    Start();
                    break;
                }
            }
        });
    
    ui->SetOnAddComponentCallback([this]() {
        ui->ShowComponentList(componentNames,
            [this](const std::string& compName) {
                auto newComp = ComponentRegistry::CreateComponentByName(compName);
                if (newComp) {
                    currentGameObject->AddComponent(newComp.release());
                    ClearInspector();
                    Start();
                }
            });
    });
}

void InspectorLogic::Update() {
    if (!currentGameObject) return;
    
    ui->HandleScroll();
    ui->UpdateComponentPositions(inspectorObjects.size());
}

void InspectorLogic::SetObject(GameObject* newCurrentObject) { 
    if (currentGameObject == newCurrentObject) return;
    
    ClearInspector();
    
    currentGameObject = newCurrentObject;
    
    if (currentGameObject) {
        Start();
    }
}

GameObject* InspectorLogic::GetCurrentObject() { 
    return currentGameObject; 
}