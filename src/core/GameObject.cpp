#include "GameObject.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <typeindex>
#include <typeinfo> 
#include "Component.h"
#include "Scene.h"

GameObject::GameObject(const Scene* scene) : scene(scene) {}

GameObject::~GameObject(){
    if (!destroyed) {
        Destroy();
    }  
}

void GameObject::Initialize() {
    for(auto& comp : components) {
        comp->Initialize();
        comp->SetGameObject(this); 
    }
}
void GameObject::Initialize(const Scene* scene) {
    SetScene(scene);
    for(auto& comp : components) {
        comp->Initialize();
        comp->SetGameObject(this); 
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

const Scene* GameObject::GetScene() { return scene; }
void GameObject::SetScene(const Scene* scene) { this->scene = scene; }