#pragma once

#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include "Scene.h"
#include "GameObject.h"
#include "ComponentLibrary.h"
#include "SerializeField.h"
#include "ResourcesTypes.h"

std::string Trim(const std::string& str);

class SceneDeserializer {
private:
    struct ParserState {
        std::unique_ptr<GameObject> currentObject;
        Component* currentComponent = nullptr;
        std::string currentCompName;

        bool inObject = false;
        bool inComponent = false;
        bool inHirarchy = false;

        bool inResources = false;

        Scene* scene = nullptr;
        
        void Reset() {
            currentObject.reset();
            currentComponent = nullptr;
            currentCompName.clear();
            inObject = false;
            inComponent = false;
        }
    };

    std::unordered_map<std::string, ResourceType> allResources;
    
    void HandleToken(const std::string& token, ParserState& state);
    void HandleKeyValue(const std::string& key, const std::string& value, ParserState& state);

    void ParseName(const std::string& value, ParserState& state);
    void ParseComponent(const std::string& key, const std::string& value, ParserState& state);
    void ParseResource(const std::string& key, const std::string& value);
    void ParseHierarchy();

public:
    SceneDeserializer() = default;
    
    SceneDeserializer(const SceneDeserializer&) = delete;
    SceneDeserializer& operator=(const SceneDeserializer&) = delete;
    
    SceneDeserializer(SceneDeserializer&&) = default;
    SceneDeserializer& operator=(SceneDeserializer&&) = default;

    std::unique_ptr<Scene> GenerateSceneThroughFile(const std::string& sceneText);
};