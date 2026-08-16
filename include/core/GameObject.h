#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <typeindex>
#include <typeinfo> 
#include <unordered_set>

class Component;
class HierarchySystem;
class Scene;

class GameObject {
private:
    std::vector<std::unique_ptr<Component>> components;
    std::unordered_map<Component*, size_t> componentIndexMap;
    std::unordered_map<std::type_index, std::unordered_set<size_t>> componentsByType;

    HierarchySystem* hierarchySystem{nullptr};

    bool initialized{false};
    bool started{false};
    bool destroyed{false}; 

    std::string name;

    Scene* scene{nullptr};

    template<typename T>
    T* AddComponentInternal(std::unique_ptr<T>&& component) {
        component->SetGameObject(this);
        T* ptr = component.get();
        
        size_t index = components.size();
        componentIndexMap[ptr] = index;
        components.push_back(std::move(component));
        componentsByType[typeid(T)].insert(index);
        
        if (initialized) ptr->Initialize();
        if (started) ptr->Start();
        
        return ptr;
    }
    
public:
    GameObject(const std::string& name = "GameObject", Scene* scene = nullptr);
    virtual ~GameObject();

    void Initialize();
    void Initialize(Scene* scene);
    
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        return AddComponentInternal(std::move(component));
    }
    
    template<typename T>
    T* AddComponent(T* component) {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        
        if (!component) return nullptr;
        
        std::unique_ptr<T> ptr(component);
        return AddComponentInternal(std::move(ptr));
    }
    
    void RemoveComponent(Component* component);

    template<typename T>
    T* GetComponentOfType() {
        if (destroyed) return nullptr;
        
        auto it = componentsByType.find(typeid(T));
        if (it != componentsByType.end() && !it->second.empty()) {
            size_t index = *it->second.begin();
            if (index < components.size()) {
                return static_cast<T*>(components[index].get());
            }
        }
        return nullptr;
    }
    
    template<typename T>
    std::vector<T*> GetComponentsOfType() {
        std::vector<T*> result;
        if (destroyed) return result;
        
        auto it = componentsByType.find(typeid(T));
        if (it != componentsByType.end()) {
            result.reserve(it->second.size());
            for (size_t index : it->second) {
                if (index < components.size()) {
                    result.push_back(static_cast<T*>(components[index].get()));
                }
            }
        }
        return result;
    }
    
    template<typename T>
    bool HasComponent() const {
        if (destroyed) return false;
        
        auto it = componentsByType.find(typeid(T));
        return it != componentsByType.end() && !it->second.empty();
    }
    
    template<typename T>
    size_t GetComponentCount() const {
        auto it = componentsByType.find(typeid(T));
        return (it != componentsByType.end()) ? it->second.size() : 0;
    }
    
    void Start();
    void Update();
    void Destroy();
    
    std::vector<Component*> GetComponents();

    std::string GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }

    void SetHierarchySystem(HierarchySystem* system) { hierarchySystem = system; }
    HierarchySystem* GetHierarchySystem() const { 
        return hierarchySystem;
    }

    Scene* GetScene() const { return scene; }
    void SetScene(Scene* scene) { this->scene = scene; }

    bool IsDestroyed() const { return destroyed; }
    bool IsInitialized() const { return initialized; }
    bool IsStarted() const { return started; }
};