#include "SceneDeserializer.h"

SceneDeserializer::~SceneDeserializer() {
    
}

void SceneDeserializer::HandleToken(const std::string& token, ParserState& state) {
    if (token == "object") {
        state.Reset();
        state.currentObject = std::make_unique<GameObject>();
    }
    else if (token == "components") {
        state.inComponents = true;
    }
    else if (token == "}") {
        if (state.inComponent && state.currentComponent) {
            state.inComponent = false;
            state.currentComponent = nullptr;
        }
        else if (state.inComponents && state.currentObject) {
            state.inComponents = false;
            if (state.scene) {
                state.scene->AddGameObject(std::move(state.currentObject));
            }
            state.currentObject.reset();
        }
    }
    else if (state.inComponents && !token.empty()) {
        if (!state.inComponent) {
            state.currentCompName = token;
            auto comp = ComponentRegistry::Create(state.currentCompName);
            if (comp && state.currentObject) {
                state.currentComponent = comp.get();
                state.currentObject->AddComponent(comp.release()); 
                state.inComponent = true;
            }
        }
    }
}

void SceneDeserializer::HandleKeyValue(const std::string& key, const std::string& value, ParserState& state) {
    if (key == "name" && !state.currentObject && state.scene) {
        state.scene->SetName(value);
        return;
    }
    
    if (key == "name" && state.currentObject && !state.inComponent) {
        state.currentObject->SetName(value);
        return;
    }
    
    if (state.currentComponent && state.inComponent) {
        if (auto* serializable = dynamic_cast<ISerializable*>(state.currentComponent)) {
            for (auto* field : serializable->GetSerializedFields()) {
                if (field->GetName() == key) {
                    field->FromString(value);
                    break;
                }
            }
        }
    }
}

std::unique_ptr<Scene> SceneDeserializer::GenerateSceneThroughFile(const std::string& sceneText) {
    auto scene = std::make_unique<Scene>();
    
    std::istringstream stream(sceneText);
    std::string line;
    
    ParserState state;
    state.scene = scene.get();
    
    while (std::getline(stream, line)) {
        size_t start = line.find_first_not_of(" /t/r/n");
        if (start == std::string::npos) continue;
        
        size_t end = line.find_last_not_of(" /t/r/n");
        std::string trimmed = line.substr(start, end - start + 1);
        
        size_t colonPos = trimmed.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trimmed.substr(0, colonPos);
            std::string value = trimmed.substr(colonPos + 1);
            
            key = Trim(key);
            value = Trim(value);
            
            HandleKeyValue(key, value, state);
        }
        else {
            HandleToken(trimmed, state);
        }
    }
    
    return scene;
}

std::string Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" /t/r/n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" /t/r/n");
    return str.substr(start, end - start + 1);
}