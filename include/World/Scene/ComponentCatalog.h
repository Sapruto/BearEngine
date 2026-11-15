#pragma once

#include "Systems/Core/HasRequired.h"

#include <string>
#include <vector>
#include <unordered_map>

class Component;

class ComponentCatalog {
private:
    std::unordered_map<std::string, Component*> uuidToComponent;
    std::vector<Component*> components;

    std::vector<Systems::HasRequired*> allRequirers;
    std::vector<size_t> indecesNewRequirers;

public:
    ComponentCatalog() = default;

    void Clear();

    void Register(Component* comp);
    void Unregister(Component* comp);

    Component* FindByUUID(const std::string& uuid) const;

    template<typename T>
    T* FindByUUID(const std::string& uuid) const {
        return dynamic_cast<T*>(FindByUUID(uuid));
    }

    const std::vector<Component*>& GetAllComponents() const { return components; }
    const std::vector<Systems::HasRequired*>& GetAllRequirers() const { return allRequirers; }

    std::vector<Systems::HasRequired*> ConsumeNewRequirers();

    std::size_t GetRequirerCount() const { return allRequirers.size(); }
    std::size_t GetComponentCount() const { return components.size(); }
};