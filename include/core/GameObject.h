#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <typeindex>
#include <typeinfo> 

class Component;
class HierarchySystem;
class Scene;

class GameObject{
private:
    std::vector<std::unique_ptr<Component>> components;

    const HierarchySystem* hierarchySystem{nullptr};

    bool destroyed{false}; 

    std::string name;

    const Scene* scene{nullptr};
    
public:
    GameObject() = default;
    GameObject(const std::string& name){
        this->name = name;
    }
    GameObject(const Scene* scene);
    
    virtual ~GameObject();

    void Initialize();
    void Initialize(const Scene* scene);
    
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args){
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        
        component->SetGameObject(this);
        T* ptr = component.get();
        
        components.push_back(std::move(component));
        
        ptr->Initialize();
        ptr->Start();
        
        return ptr;
    }
    
    void RemoveComponent(Component* component);

    //To delete
    template<typename T>
    T* AddComponent(T* component) {
        component->SetGameObject(this);
        T* rawPtr = component;
        components.push_back(std::unique_ptr<Component>(component));
        
        rawPtr->Initialize();
        rawPtr->Start();
        
        return rawPtr;
    }

    template<typename T>
    T* GetComponentOfType(){
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

    void SetHierarchySystem(const HierarchySystem* system) { hierarchySystem = system; }
    const HierarchySystem* GetHierarchySystem() { 
        if(hierarchySystem) return hierarchySystem;
        return nullptr;
    }

    const Scene* GetScene();
    void SetScene(const Scene* scene);
};