#pragma once

class GameObject;

class Component {
protected:
    GameObject* gameObject;
    
public:
    Component() : gameObject(nullptr) {}
    
    virtual ~Component() {}
    
    void SetGameObject(GameObject* obj) { 
        gameObject = obj; 
    }
    
    void ClearGameObject() { 
        gameObject = nullptr; 
    }
    
    virtual void Start() {
        if (!gameObject) return;
    }
    
    virtual void Update(float deltaTime) {
        if (!gameObject) return;
    }
    
    virtual void Destroy() {}

    virtual void OnCollisionEnter(GameObject* other) {
        if (!gameObject) return;
    }
    
    GameObject* GetGameObject() const { 
        return gameObject; 
    }
    
    bool IsValid() const {
        return gameObject != nullptr;
    }
};