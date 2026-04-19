#include "Inspector.h"

#include "GraphicsUtilits/UI/UtilitUI.h"

#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "SerializeField.h"

void Inspector::StartInspectOfObject(){
    if (!currentGameObject) return;

    for(auto [gameObject, _] : inspectorObjects){
        scene->RemoveGameObject(gameObject);
    }
    inspectorObjects.clear();

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
}

void Inspector::UpdateInspectOfObject() {
    if (!currentGameObject) return;
    
    for (auto& [obj, data] : inspectorObjects) {
        if (obj) {
            scene->RemoveGameObject(obj);
            delete obj;
        }
    }
    inspectorObjects.clear();
    
    if(gameObjectIsChanged){
        componentNames = ComponentRegistry::GetAllNames();
        
        std::vector<Component*> components = currentGameObject->GetComponents();
        
        for(auto* comp : components){
            std::string name = ComponentRegistry::GetNameByComponent(comp);
            DrawComponentData data = DrawComponentData(name);
            
            if (auto* serializable = dynamic_cast<ISerializable*>(comp)) {
                for (auto* field : serializable->GetSerializedFields()) {
                    data.nameToParams[field->GetName()] = field->ToString();
                }
            }
            GameObject* object = scene->CreateGameObject();
            inspectorObjects[object] = data;
        }
        
        gameObjectIsChanged = false;
    } 
    else {
        std::vector<Component*> components = currentGameObject->GetComponents();
        int idx = 0;
        for(auto* comp : components){
            if (idx >= inspectorObjects.size()) break;
            
            auto it = inspectorObjects.begin();
            std::advance(it, idx);
            
            if (auto* serializable = dynamic_cast<ISerializable*>(comp)) {
                for (auto* field : serializable->GetSerializedFields()) {
                    it->second.nameToParams[field->GetName()] = field->ToString();
                }
            }
            idx++;
        }
    }
    
    ui->BeginDraw();
    ui->ShowObjectComponents(currentGameObject, inspectorObjects);
    
    if (isShowAllComps) {
        ui->ShowAllComponents(currentGameObject, componentNames);
    }
}

void Inspector::SetObject(GameObject* newCurrentObject) { currentGameObject = newCurrentObject; gameObjectIsChanged = true; }
GameObject* Inspector::GetCurrentObject() { return currentGameObject; }