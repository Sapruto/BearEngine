#pragma once

#include <vector>
#include <string>
#include <memory>

class Scene;
class SceneDeserializer;

class SceneManager {
private:
    SceneDeserializer& m_deserializer;
    std::string m_scenesPath;
    std::vector<std::unique_ptr<Scene>> m_scenes;
    Scene* m_currentScene = nullptr;
    
    Scene* FindScene(const std::string& name) const;
    
public:
    explicit SceneManager(SceneDeserializer& deserializer);
    
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    
    SceneManager(SceneManager&&) = default;
    SceneManager& operator=(SceneManager&&) = default;
    
    ~SceneManager() = default;
    
    void Initialize(const std::string& scenesPath);
    
    bool LoadScenes();
    
    bool SwitchToScene(int index);
    bool SwitchToScene(const std::string& sceneName);
    
    Scene* GetCurrentScene();
    const Scene* GetCurrentScene() const;
    
    void UpdateCurrentScene();
};