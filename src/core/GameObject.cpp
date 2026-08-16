#include "GameObject.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <typeindex>
#include <typeinfo> 
#include "Component.h"
#include "Scene.h"

GameObject::GameObject(const std::string& name, Scene* scene) 
: name(name), scene(scene) {}

GameObject::~GameObject(){
    if (!destroyed) {
        Destroy();
    }  
}

void GameObject::Initialize() {
    if (initialized) return;

    for(auto& comp : components) {
        if (comp) {
            comp->Initialize();
            comp->SetGameObject(this);
        }
    }
    initialized = true;
}
void GameObject::Initialize(Scene* scene) {
    if (initialized) return;

    SetScene(scene);
    for(auto& comp : components) {
        if (comp) {
            comp->Initialize();
            comp->SetGameObject(this);
        }
    }
    initialized = true;
}

void GameObject::RemoveComponent(Component* component) {
    if (!component || destroyed || components.empty()) return;
    
    auto it = componentIndexMap.find(component);
    if (it == componentIndexMap.end()) return;
    
    size_t index = it->second;
    size_t lastIndex = components.size() - 1;
    
    component->Destroy();
    component->ClearGameObject();
    
    componentIndexMap.erase(it);
    
    auto typeIt = componentsByType.find(typeid(*component));
    if (typeIt != componentsByType.end()) {
        typeIt->second.erase(index);
        if (typeIt->second.empty()) {
            componentsByType.erase(typeIt);
        }
    }
    
    if (index != lastIndex) {
        std::swap(components[index], components.back());
        Component* movedComponent = components[index].get();
        componentIndexMap[movedComponent] = index;
        
        std::type_index movedType = typeid(*movedComponent);
        auto& set = componentsByType[movedType];
        set.erase(lastIndex);
        set.insert(index);
    }
    
    components.pop_back();
}

void GameObject::Start() {
    if (started) return;

    for (auto& comp : components) {
        if (comp) {
            comp->Start();
        }
    }
    started = true;
}

void GameObject::Update() {
    for (auto& comp : components) {
        if (comp) {
            comp->Update();
        }
    }
}

void GameObject::Destroy() {
    if (destroyed) return;
    
    for (auto& comp : components) {
        if (comp) {
            comp->Destroy();
            comp->ClearGameObject();
        }
    }
    
    destroyed = true;
    components.clear();
}


std::vector<Component*> GameObject::GetComponents() {
    std::vector<Component*> result;
    result.reserve(components.size());
    
    for (const auto& comp : components) {
        result.push_back(comp.get());
    }
    
    return result;
}