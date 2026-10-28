#pragma once

#include "World/Scene/SceneEvent.h"
#include "Domains/Core/DomainRoot.h"

#include <vector>
#include <memory>

class GameObject;

class ComponentInjector {
public:
    void InjectAll(Domains::DomainRoot& domainRoot,
                            const std::vector<GameObject*>& gameObjects) {
        for (auto& domain : domainRoot.GetAllDomains()) {
            for (auto* gameObject : gameObjects) {
                if (gameObject) domain->AddByObject(gameObject);
            }
        }
    }

    void UninjectAll(Domains::DomainRoot& domainRoot,
                            const std::vector<GameObject*>& gameObjects) {
        for (auto& domain : domainRoot.GetAllDomains()) {
            for (auto* gameObject : gameObjects) {
                if (gameObject) domain->RemoveByObject(gameObject);
            }
        }
    }
};

class ComponentInjectorEvent : public SceneEventT<ComponentInjectorEvent> {
private:
    Domains::DomainRoot& domainRoot;

public:
    explicit ComponentInjectorEvent(Domains::DomainRoot& root) : domainRoot(root) {}

    void OnAddObject(GameObject* gameObject) override {
        if (!gameObject) return;
        for (auto& domain : domainRoot.GetAllDomains()) {
            domain->AddByObject(gameObject);
        }
    }

    void OnRemoveObject(GameObject* gameObject) override {
        if (!gameObject) return;
        for (auto& domain : domainRoot.GetAllDomains()) {
            domain->RemoveByObject(gameObject);
        }
    }
};