#include "SceneCreator.h"

#include <vector>
#include <iostream> 
#include <fstream>
#include <memory>
#include <string>
#include <chrono>
#include <time.h>
#include <filesystem>

#include "Scene.h"
#include "SceneDeserializer.h"
#include "SceneSerializer.h"

using namespace std;
namespace fs = std::filesystem;

SceneCreator::SceneCreator(SceneSerializer& serializer, SceneDeserializer& deserializer) 
    : serializer(serializer), deserializer(deserializer)
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
bool SceneCreator::UpdateSceneFile(const std::string& filePath, const Scene& scene) {
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