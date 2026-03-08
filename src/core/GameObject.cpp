#include "GameObject.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <typeindex>
#include <typeinfo> 
#include "Component.h"

GameObject::~GameObject(){
    if (!destroyed) {
        Destroy();
    }  
}

void GameObject::RemoveComponent(Component* component) {
    auto it = std::find_if(components.begin(), components.end(),
        [component](const std::unique_ptr<Component>& comp) {
            return comp.get() == component;
        });
    
    if (it != components.end()) {
        (*it)->Destroy();
        (*it)->ClearGameObject();
        components.erase(it);
    }
}

void GameObject::Start() {
    for (auto& comp : components) {
        if(comp){
            comp->Start();
        }
    }
}

void GameObject::Update() {
    for (auto& comp : components) {
        if(comp){
            comp->Update();
        }
    }
}

void GameObject::Destroy() {
    if (destroyed) {
        return;
    }
    
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