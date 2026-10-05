#include "Systems/Serialization/SceneParser/SceneCreator.h"

#include <vector>
#include <iostream> 
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

#include "Scene.h"
#include "Systems/Serialization/SceneParser/SceneDeserializer.h"
#include "Systems/Serialization/SceneParser/SceneSerializer.h"

#include "Systems/Serialization/Chunks/ResourceParser.h"
#include "Systems/Serialization/Chunks/ObjectParser.h"
#include "Systems/Serialization/Chunks/ComponentsParser.h"
#include "Systems/Serialization/Chunks/ComponentParser.h"
#include "Systems/Serialization/Chunks/HierarchyParser.h"

namespace fs = std::filesystem;
using namespace Serialization;
using namespace Serialization::Chunks;

std::vector<std::shared_ptr<IChankParser>> SceneCreator::MakeDefaultParsers() {
    return {
        std::make_shared<ResourceParser>(),
        std::make_shared<ObjectParser>(),
        std::make_shared<ComponentsParser>(),
        std::make_shared<ComponentParser>(),
        std::make_shared<HierarchyParser>(),
    };
}

SceneCreator::SceneCreator(std::vector<std::shared_ptr<IChankParser>> p)
    : parsers(std::move(p)),
      serializer(parsers),
      deserializer(parsers)
{}

SceneCreator::~SceneCreator() {

}

bool SceneCreator::CreateNewSceneFile(const std::string& path, const std::string& name){
    fs::path fullPath = fs::path(path) / name;
    
    fs::create_directories(fs::path(path));
    
    std::ofstream file(fullPath);
    
    if (file.is_open()) {
        file.close();
        return true;
    } 
    return false;
}

bool SceneCreator::UpdateSceneFile(const std::string& filePath, Scene& scene) {
    std::string newScene = serializer.GenerateTextThroughScene(scene);

    std::ofstream file(filePath, std::ios::trunc);
    
    if (file) {
        file << newScene;
        return true;
    }
    return false;
}

bool SceneCreator::DeleteSceneFile(const std::string& filePath){
    try {
        if (fs::remove(filePath)) {
            return true;
        } 
        else {
            return false;
        }
    } 
    catch (const fs::filesystem_error& e) {
        return false;
    }
    return false;
}

std::vector<std::unique_ptr<Scene>> SceneCreator::GetScenes(const std::string& pathDirectory) {
    std::vector<std::unique_ptr<Scene>> scenes;
    
    if (!fs::exists(pathDirectory) || !fs::is_directory(pathDirectory)) {
        return scenes;
    }
    
    for (const auto& entry : fs::directory_iterator(pathDirectory)) {
        if (fs::is_regular_file(entry.path())) {
            std::string extension = entry.path().extension().string();
            if (extension == ".scene") {
                std::unique_ptr<Scene> scene = GetScene(entry.path().string());
                if (scene) {
                    scenes.push_back(std::move(scene));
                }
            }
        }
    }
    
    return scenes;
}

std::unique_ptr<Scene> SceneCreator::GetScene(const std::string& pathFile) {
    if (!fs::exists(pathFile) || !fs::is_regular_file(pathFile)) {
        return nullptr;
    }
    
    std::ifstream file(pathFile);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sceneText = buffer.str();
    file.close();
    
    if (sceneText.empty()) {
        return nullptr;
    }
    
    return deserializer.GenerateSceneThroughFile(sceneText);
}

std::string SceneCreator::SerializeToString(Scene& scene) {
    return serializer.GenerateTextThroughScene(scene);
}

std::unique_ptr<Scene> SceneCreator::DeserializeFromString(const std::string& text) {
    return deserializer.GenerateSceneThroughFile(text);
}