#pragma once

#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include "Scene.h"
#include "GameObject.h"
#include "ComponentLibrary.h"
#include "SerializeField.h"

std::string Trim(const std::string& str);

class SceneDeserializer {
private:
    struct ParserState {
        std::unique_ptr<GameObject> currentObject;
        Component* currentComponent = nullptr;
        std::string currentCompName;
        bool inComponents = false;
        bool inComponent = false;
        Scene* scene = nullptr;
        
        void Reset() {
            currentObject.reset();
            currentComponent = nullptr;
            currentCompName.clear();
            inComponents = false;
            inComponent = false;
        }
    };
    
    void HandleToken(const std::string& token, ParserState& state);
    void HandleKeyValue(const std::string& key, const std::string& value, ParserState& state);

public:
    SceneDeserializer() = default;
    ~SceneDeserializer();
    
    SceneDeserializer(const SceneDeserializer&) = delete;
    SceneDeserializer& operator=(const SceneDeserializer&) = delete;
    
    SceneDeserializer(SceneDeserializer&&) = default;
    SceneDeserializer& operator=(SceneDeserializer&&) = default;

    std::unique_ptr<Scene> GenerateSceneThroughFile(const std::string& sceneText);
};