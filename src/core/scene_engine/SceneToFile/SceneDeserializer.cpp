#include "SceneDeserializer.h"

#include "SceneTokens.h"
#include "ResourceManager.h"

void SceneDeserializer::HandleToken(const std::string& token, ParserState& state) {
    if (token == SceneTokens::resources_pathes) {
        state.inResources = true;
    }
    else if (token == SceneTokens::object) {
        state.Reset();
        state.inObject = true;
        state.currentObject = std::make_unique<GameObject>();
    }
    else if (token == SceneTokens::components) {
        state.inComponent = true;
    }
    else if(token == SceneTokens::hierarchy){
        state.inHierarchy = true;
    }
    else if (token == SceneTokens::end_part) {
        if (state.inComponent && state.currentComponent) {
            state.inComponent = false;
            state.currentComponent = nullptr;
        }
        else if (state.inObject && state.currentObject) {
            state.inObject = false;
            if (state.scene) {
                state.scene->AddGameObject(std::move(state.currentObject));
            }
            state.currentObject.reset();
        }
        else if(state.inHierarchy){
            state.inHierarchy = false;
        }
        else if (state.inResources) {
            state.inResources = false;
        }
    }
    else if (state.inObject && !token.empty()) {
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

void SceneDeserializer::ParseName(const std::string& value, ParserState& state){
    if (!state.currentObject && state.scene) {
        state.scene->SetName(value);
    }
    else if (state.currentObject && !state.inComponent) {
        state.currentObject->SetName(value);
    }
}
void SceneDeserializer::ParseComponent(const std::string& key, const std::string& value, ParserState& state){
    if (state.currentComponent) {
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
void SceneDeserializer::ParseResource(const std::string& key, const std::string& value){
    allResources.insert({value, ResourceTypeUtils::FromString(key)});
}

void SceneDeserializer::ParseHierarchy(ParserState& state){
    HierarchySystem* system = state.scene->GetHierarchySystem();
    if(!system || state.currentObject) return;

    system->AddGameObject(state.currentObject.get());
}

void SceneDeserializer::HandleKeyValue(const std::string& key, const std::string& value, ParserState& state) {
    if (key == SceneTokens::name) {
        ParseName(value, state);
    }
    else if (state.inComponent) {
        ParseComponent(key, value, state);
    }
    else if(state.inResources){
        ParseResource(key, value);
    }
    else if(state.inHierarchy){
        ParseHierarchy(state);
    }
}

std::unique_ptr<Scene> SceneDeserializer::GenerateSceneThroughFile(const std::string& sceneText) {
    auto scene = std::make_unique<Scene>();
    
    std::istringstream stream(sceneText);
    std::string line;
    
    ParserState state;
    state.scene = scene.get();
    
    while(std::getline(stream, line)){
        std::string trimmed = Trim(line);
        if (trimmed.empty()) continue;
        
        if (trimmed == SceneTokens::end_part){
            HandleToken(std::string(SceneTokens::end_part), state);
            continue;
        }
        
        if (trimmed.back() == SceneTokens::start_part[0]){
            std::string token = Trim(trimmed.substr(0, trimmed.length() - 1));
            HandleToken(token, state);
            continue;
        }
        
        size_t spacePos = trimmed.find(' ');
        if(spacePos != std::string::npos){
            std::string key = Trim(trimmed.substr(0, spacePos));
            std::string value = Trim(trimmed.substr(spacePos + 1));
            HandleKeyValue(key, value, state);
        }
        else{
            HandleToken(trimmed, state);
        }
    }
    scene->AddResources(allResources);

    return scene;
}

std::string Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}