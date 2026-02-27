#include "include/core/scene_engine/SceneManager.h"
#include "include/core/scene_engine/Scene.h"
#include "include/core/scene_engine/SceneToFile/SceneDeserializer.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

SceneManager::SceneManager(SceneDeserializer& deserializer) 
    : m_deserializer(deserializer) 
{}

void SceneManager::Initialize(const std::string& scenesPath) {
    m_scenesPath = scenesPath;
}

bool SceneManager::LoadScenes() {
    m_scenes.clear();
    m_currentScene = nullptr;
    
    try {
        if (!fs::exists(m_scenesPath)) {
            fs::create_directories(m_scenesPath);
            return true;
        }
        
        for (const auto& entry : fs::directory_iterator(m_scenesPath)) {
            if (entry.path().extension() != ".scene") continue;
            
            std::ifstream file(entry.path());
            if (!file) {
                continue;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            
            auto scene = m_deserializer.GenerateSceneThroughFile(buffer.str());
            if (scene) {
                m_scenes.push_back(std::move(scene));
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool SceneManager::SwitchToScene(int index) {
    if (index < 0 || index >= static_cast<int>(m_scenes.size())) {
        return false;
    }
    
    Scene* newScene = m_scenes[index].get();
    if (newScene == m_currentScene) {
        return true; 
    }
    
    if (m_currentScene) {
        m_currentScene->SetActive(false);
    }
    
    m_currentScene = newScene;
    if (m_currentScene) {
        m_currentScene->SetActive(true);
    }
    
    return true;
}

bool SceneManager::SwitchToScene(const std::string& sceneName) {
    Scene* newScene = FindScene(sceneName);
    if (!newScene || newScene == m_currentScene) {
        return false;
    }
    
    if (m_currentScene) {
        m_currentScene->SetActive(false);
    }
    
    m_currentScene = newScene;
    m_currentScene->SetActive(true);
    
    return true;
}

Scene* SceneManager::FindScene(const std::string& name) const {
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&name](const auto& scene) {
            return scene && scene->GetName() == name;
        });
    
    return it != m_scenes.end() ? it->get() : nullptr;
}

Scene* SceneManager::GetCurrentScene() {
    return m_currentScene;
}

const Scene* SceneManager::GetCurrentScene() const {
    return m_currentScene;
}

void SceneManager::UpdateCurrentScene() {
    if (m_currentScene) {
        m_currentScene->UpdateScene();
    }
}