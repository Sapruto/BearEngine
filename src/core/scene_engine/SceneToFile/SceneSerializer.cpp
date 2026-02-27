#include "include/core/scene_engine/SceneToFile/SceneSerializer.h"

#include <filesystem> 

namespace fs = std::filesystem;

SceneSerializer::~SceneSerializer() {

}

std::string SceneSerializer::GenerateTextThroughScene(const Scene& scene){
    std::string result;
    
    result += "name : " + scene.GetName() + "\n";
    result += "{\n";
    result += "    game_objects\n";
    result += "    {\n";
    
    for (const auto& obj : *scene.GetGameObjects()) {
        result += "        object\n";
        result += "        {\n";
        result += "            name : " + obj->GetName() + "\n";
        result += "            components\n";
        result += "            {\n";
        
        for (const auto* comp : obj->GetComponents()) {
            result += "                " + ComponentRegistry::GetNameByComponent(comp) + "\n";
            result += "                {\n";
            
            if (auto* serializable = dynamic_cast<ISerializable*>(const_cast<Component*>(comp))) {
                for (const auto* field : serializable->GetSerializedFields()) {
                    result += "                    " + field->GetName() + " : " + field->ToString() + "\n";
                }
            }
            
            result += "                }\n";
        }
        
        result += "            }\n";
        result += "        }\n";
    }
    
    result += "    }\n";
    result += "}\n";
    
    return result;
}