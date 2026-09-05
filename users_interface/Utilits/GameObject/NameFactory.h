#pragma once

#include <string>
#include <vector>
#include <memory>

#include "GameObject.h"
#include "Scene.h"


class NameFactory{
private:
    std::string prefix{"_"};

public:
    void MakeUniqueNamesOnScene(const Scene& scene){
        std::vector<GameObject*> objects = scene.GetGameObjects();

        std::unordered_map<std::string, int> nameCount;
        std::unordered_map<GameObject*, std::string> objectToName;
        
        for (auto* object : objects) {
            std::string name = object->GetName();
            objectToName[object] = name;
            nameCount[name]++;
        }
        
        std::unordered_map<std::string, int> nameIndex;
        
        for (auto* object : objects) {
            std::string originalName = objectToName[object];
            
            if (nameCount[originalName] > 1) {
                std::string newName = originalName + prefix + std::to_string(nameIndex[originalName]);
                object->SetName(newName);
                nameIndex[originalName]++;
            }
        }
    }
};