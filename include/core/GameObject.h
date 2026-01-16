#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <typeindex>
#include <typeinfo> 
#include "include/core/Component.h"

class GameObject {
private:
    std::vector<Component*> components;

    bool destroyed = false; 
    
public:
    GameObject() = default;
    
    virtual ~GameObject();
    
    Component* AddComponent(Component* component);
    
    void RemoveComponent(Component* component);

    template<typename T>
    T* GetComponentOfType() {
        if (destroyed) return nullptr;
        
        static std::type_index typeIdx = typeid(T);
        
        for (Component* comp : components) {
            if (typeid(*comp) == typeid(T)) {
                return static_cast<T*>(comp);
            }
        }
        return nullptr;
    }
    
    void Start();
    
    void Update(float deltaTime);

    void Destroy();
    
    const std::vector<Component*>& GetComponents();

    bool IsDestroyed() const { return destroyed; }
};