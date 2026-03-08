#pragma once

#include "GameObject.h"
#include <vector>

class Component {
protected:
    GameObject* gameObject;
    
public:
    Component() : gameObject(nullptr) {}
    
    virtual ~Component() {}
    
    void SetGameObject(GameObject* obj) { 
        gameObject = obj; 
    }
    
    void ClearGameObject(){
        gameObject = nullptr;
    }
    
    virtual void Start(){}
    
    virtual void Update(){}
    
    virtual void Destroy(){}

    virtual void OnCollisionEnter(GameObject* other){}
    
    GameObject* GetGameObject() const { 
        return gameObject; 
    }
    
    bool IsValid() const {
        return gameObject != nullptr;
    }
};