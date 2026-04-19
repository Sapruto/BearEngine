#pragma once

#include <memory>

#include "GameObject.h"

struct HierarchyNode {
    GameObject* object;
    std::vector<uint32_t> parentsIds;
    std::vector<uint32_t> childrenIds;

    HierarchyNode() = default;
    HierarchyNode(GameObject* object) : object(object) {}
};

enum class DeleteStrategy {
    CASCADE,   
    REATTACH_TO_PARENT 
};

class HierarchySystem {
    std::unordered_map<uint32_t, HierarchyNode> nodes;
    std::unordered_map<GameObject*, uint32_t> objectToId; 

    uint32_t nextId = 1;

    void AddUniqueId(std::vector<uint32_t>& vec, uint32_t id);

    HierarchyNode* FindNode(GameObject* obj);
    std::vector<GameObject*> GetObjectsByIds(std::vector<uint32_t> ids);

    void RemoveGameObjectRecursive(GameObject* gameObjectToDelete, DeleteStrategy strategy);

public:
    HierarchySystem() = default;
    ~HierarchySystem() = default;

    void AddGameObject(GameObject* newGameObject);

    void AddCommunication(GameObject* object, std::vector<GameObject*> others, bool isChildren);
    void RemoveCommunication(GameObject* object, GameObject* other, bool isChildren);

    void RemoveGameObject(GameObject* gameObjectToDelete, DeleteStrategy strategy);

    GameObject* GetParent(GameObject* gameObject, int index = 0);
    std::vector<GameObject*> GetParents(GameObject* gameObject);

    GameObject* GetChild(GameObject* gameObject, int index = 0);
    std::vector<GameObject*> GetChilds(GameObject* gameObject);

    std::unordered_map<int, std::vector<GameObject*>> GetParentTree(GameObject* gameObject, int layers);
    std::unordered_map<int, std::vector<GameObject*>> GetChildrenTree(GameObject* gameObject, int layers);
};