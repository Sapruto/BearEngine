/*#pragma once

#include <memory>

#include "GameObject.h"

class HierarchySystem {
    struct HierarchyNode {
        GameObject* obj;
        uint32_t parentId;
        std::vector<uint32_t> childrenIds;
    };

    std::unordered_map<uint32_t, HierarchyNode> nodes;
    uint32_t nextId = 1;

public:
    HierarchySystem() = default;
    ~HierarchySystem() = default;

    void AddGameObject(GameObject* newGameObject);
    void AddCommunication(GameObject* parent, GameObject* child);

    void RemoveGameObject(GameObject* gameObjectToDelete);

    GameObject* GetParent(GameObject* gameObject);
    std::vector<GameObject*> GetParents(GameObject* gameObject);

    GameObject* GetChild(GameObject* gameObject);
    std::vector<GameObject*> GetChilds(GameObject* gameObject);
};*/