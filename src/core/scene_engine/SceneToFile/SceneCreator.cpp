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

SceneCreator::SceneCreator(SceneSerializer& serializer) 
    : m_serializer(serializer) 
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
    std::string newScene = m_serializer.GenerateTextThroughScene(scene);

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

std::vector<std::unique_ptr<Scene>> GetScenes(const std::string& pathDirectory) {
    return {};
}