#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

class Scene;
class SceneDeserializer;

class SceneManager {
private:
    SceneDeserializer& m_deserializer;
    std::string m_scenesPath;
    std::vector<std::unique_ptr<Scene>> m_scenes;
    Scene* m_currentScene = nullptr;
    
    std::unordered_map<std::string, std::function<std::unique_ptr<Scene>()>> m_sceneFactories;
    
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
    
    void RegisterSceneFactory(const std::string& name, std::function<std::unique_ptr<Scene>()> factory);
    
    bool CreateScene(const std::string& name);
    
    bool SwitchToScene(int index);
    bool SwitchToScene(const std::string& sceneName);
    
    Scene* GetCurrentScene();
    const Scene* GetCurrentScene() const;
    
    void UpdateCurrentScene();
    
    void UnloadScene(const std::string& name);
    void UnloadAllScenes();
    bool HasScene(const std::string& name) const;
    std::vector<std::string> GetSceneNames() const;
};