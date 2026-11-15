#pragma once

#include <atomic>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include "World/GameObject.h"

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

    std::atomic<uint32_t> nextId = 1;

    void AddUniqueId(std::vector<uint32_t>& vec, uint32_t id);
    void AddUniqueObject(std::vector<GameObject*>& vec, GameObject* obj);

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

    GameObject* GetParent(GameObject* gameObject, unsigned int index = 0);
    std::vector<GameObject*> GetParents(GameObject* gameObject);

    GameObject* GetChild(GameObject* gameObject, unsigned int index = 0);
    std::vector<GameObject*> GetChilds(GameObject* gameObject);

    std::unordered_map<int, std::vector<GameObject*>> GetParentTree(GameObject* gameObject, unsigned int layers);
    std::unordered_map<int, std::vector<GameObject*>> GetChildrenTree(GameObject* gameObject, unsigned int layers);
    std::unordered_map<int, std::vector<GameObject*>> GetTreeAroundObject(GameObject* gameObject, unsigned int up, unsigned int down);
    
    std::unordered_map<int, std::vector<GameObject*>> GetAllTree();
};