#include "SceneSerializer.h"

#include <chrono>      
#include <filesystem> 

#include "SceneSerializer.h"

#include "SceneTokens.h"

namespace fs = std::filesystem;

SceneSerializer::~SceneSerializer() {

}

std::string SceneSerializer::GenerateTextThroughScene(const Scene& scene) {
    std::string result;
    
    result += std::string(SceneTokens::resources_pathes) + " " + std::string(SceneTokens::start_part) + "\n";
    result += std::string(SceneTokens::end_part) + "\n\n";
    
    for (auto* obj : scene.GetGameObjects()) {
        result += std::string(SceneTokens::object) + " " + std::string(SceneTokens::start_part) + "\n";
        result += "    " + std::string(SceneTokens::name) + " " + obj->GetName() + "\n";
        result += "    " + std::string(SceneTokens::components) + " " + std::string(SceneTokens::start_part) + "\n";
        
        for (const auto* comp : obj->GetComponents()) {
            std::string compName = ComponentRegistry::GetNameByComponent(comp);
            result += "        " + compName + " " + std::string(SceneTokens::start_part) + "\n";
            
            if (auto* serializable = dynamic_cast<ISerializable*>(const_cast<Component*>(comp))) {
                for (const auto* field : serializable->GetSerializedFields()) {
                    result += "            " + field->GetName() + " " + field->ToString() + "\n";
                }
            }
            
            result += "        " + std::string(SceneTokens::end_part) + "\n";
        }
        
        result += "    " + std::string(SceneTokens::end_part) + "\n";
        result += std::string(SceneTokens::end_part) + "\n\n";
    }
    
    return result;
}