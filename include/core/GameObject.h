#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <typeindex>
#include <typeinfo> 
#include "include/core/system_engine/component_system/Component.h"

class GameObject {
private:
    std::vector<std::unique_ptr<Component>> components;

    bool destroyed = false; 

    std::string name;
    
public:
    GameObject() = default;
    
    virtual ~GameObject();
    
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        
        component->SetGameObject(this);
        T* ptr = component.get();
        
        components.push_back(std::move(component));
        
        ptr->Start();
        
        return ptr;
    }
    
    void RemoveComponent(Component* component);

    //To delete
    template<typename T>
    T* AddComponent(T* component) {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        
        std::unique_ptr<T> ptr(component);
        ptr->SetGameObject(this);
        T* rawPtr = ptr.get();
        
        components.push_back(std::move(ptr));
        
        rawPtr->Start();
        
        return rawPtr;
    }

    template<typename T>
    T* GetComponentOfType() {
        if (destroyed) return nullptr;
        
        static std::type_index typeIdx = typeid(T);
        
        for (const auto& comp : components) {
            if (typeid(*comp) == typeid(T)) {
                return static_cast<T*>(comp.get());
            }
        }
        return nullptr;
    }
    
    void Start();
    
    void Update();

    void Destroy();
    
    std::vector<Component*> GetComponents();

    bool IsDestroyed() const { return destroyed; }

    std::string GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
};