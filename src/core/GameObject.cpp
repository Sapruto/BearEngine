#include "include/core/GameObject.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <typeindex>
#include <typeinfo> 
#include "include/core/Component.h"

GameObject::~GameObject(){
    if (!destroyed) {
        Destroy();
    }  
}

Component* GameObject::AddComponent(Component* component) {
    if (!component) return nullptr;
    
    component->SetGameObject(this);
    components.push_back(component);
    component->Start();
    return component;
}
void GameObject::RemoveComponent(Component* component) {
    auto it = std::find(components.begin(), components.end(), component);
    if (it != components.end()) {
        (*it)->Destroy();
        (*it)->ClearGameObject();
        delete *it;
        components.erase(it);
    }
}

void GameObject::Start() {
    for (auto comp : components) {
        comp->Start();
    }
}

void GameObject::Update(float deltaTime) {
    for (auto comp : components) {
        comp->Update(deltaTime);
    }
}

void GameObject::Destroy() {
    if (destroyed) {
        return;
    }
    
    destroyed = true;
    
    for (int i = components.size() - 1; i >= 0; i--) {
        if (components[i]) {
            components[i]->Destroy();
            components[i]->ClearGameObject();
            components[i] = nullptr;
        }
    }
    components.clear();
}

const std::vector<Component*>& GameObject::GetComponents() {
    return components;
}