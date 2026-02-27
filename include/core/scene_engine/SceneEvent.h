#pragma once

class Scene;

enum class TypeSceneEvent{
    toStart,
    toUpdate,
    toDestroy
};

class SceneEvent{
protected:
    Scene* scene;
    
public:
    SceneEvent() : scene(nullptr) {}
    
    virtual ~SceneEvent() {}
    
    void SetScene(Scene* scene) { 
        this->scene = scene; 
    }
    
    void ClearGameObject() { 
        scene = nullptr; 
    }
    
    virtual void Start() {
        if (!scene) return;
    }
    
    virtual void Update() {
        if (!scene) return;
    }
    
    virtual void Destroy() {}

    virtual void OnCollisionEnter(Scene* other) {
        if (!scene) return;
    }
    
    Scene* GetGameObject() const { 
        return scene; 
    }
    
    bool IsValid() const {
        return scene != nullptr;
    }

    void operator()() {
        Update();
    }

    explicit operator bool() const {
        return scene != nullptr;
    }
};