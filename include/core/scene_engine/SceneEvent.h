#pragma once

#include <string>
#include <unordered_map>

class Scene;

class SceneEvent {
protected:
    Scene* scene{nullptr};
    std::string name;
    bool isActive = true;
    bool isEnabled = true;
    
    std::unordered_map<std::string, std::string> properties;

public:
    SceneEvent() = default;
    SceneEvent(const std::string& eventName) : name(eventName) {}
    virtual ~SceneEvent() = default;
    
    virtual void OnSceneStart() {}
    virtual void OnSceneUpdate() {}
    virtual void OnSceneDestroy() {}
    
    void SetScene(Scene* scene) { this->scene = scene; }
    void ClearScene() { scene = nullptr; }
    Scene* GetScene() const { return scene; }
    
    void SetName(const std::string& newName) { name = newName; }
    std::string GetName() const { return name; }
    
    void SetActive(bool active) { isActive = active; }
    bool IsActive() const { return isActive; }
    
    void SetEnabled(bool enabled) { isEnabled = enabled; }
    bool IsEnabled() const { return isEnabled; }
    
    void SetProperty(const std::string& key, const std::string& value) {
        properties[key] = value;
    }
    std::string GetProperty(const std::string& key) const {
        auto it = properties.find(key);
        return it != properties.end() ? it->second : "";
    }
    
    bool IsValid() const { return scene != nullptr; }
    
    virtual std::string Serialize() const { return ""; }
    virtual void Deserialize(const std::string& data) {}
};