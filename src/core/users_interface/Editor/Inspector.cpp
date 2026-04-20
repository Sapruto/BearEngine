#include "Inspector.h"

#include "GraphicsUtilits/UI/InspectorUI.h"

#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "SerializeField.h"

void Inspector::ClearInspector(){
    ui->ClearHelperObjects();
    for (auto& [obj, data] : inspectorObjects) {
        if (obj) {
            ui->DestroyUIObject(obj);
        }
    }
    inspectorObjects.clear();
}

void Inspector::StartInspectOfObject(){
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
    ui->CreateObjectComponents(currentGameObject, inspectorObjects);
}

void Inspector::UpdateInspectOfObject() {
    if (!currentGameObject) return;
    
    ui->HandleScroll();

    int index = 0;
    for (auto& [obj, data] : inspectorObjects) {
        ui->UpdateUIObjectPosition(obj, index);
        ui->UpdateUIObjectText(obj, data.componentName);
        index++;
    }
}

void Inspector::SetObject(GameObject* newCurrentObject) { 
    if (currentGameObject == newCurrentObject) return;
    
    ClearInspector();
    
    currentGameObject = newCurrentObject;
    
    if (currentGameObject) {
        StartInspectOfObject();
    }
}

GameObject* Inspector::GetCurrentObject() { 
    return currentGameObject; 
}