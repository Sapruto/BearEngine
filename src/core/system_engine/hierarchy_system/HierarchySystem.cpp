#include "HierarchySystem.h"

#include <algorithm>

#include "Component.h"

HierarchyNode* HierarchySystem::FindNode(GameObject* obj){
    auto idIt = objectToId.find(obj);
    if (idIt == objectToId.end()) return nullptr;
    
    auto nodeIt = nodes.find(idIt->second);
    return nodeIt != nodes.end() ? &nodeIt->second : nullptr;
}

std::vector<GameObject*> HierarchySystem::GetObjectsByIds(std::vector<uint32_t> ids){
    std::vector<GameObject*> objects;
    objects.reserve(ids.size());
    for(auto& id : ids){
        objects.push_back(nodes[id].object);
    }
    return objects;
}


void HierarchySystem::AddGameObject(GameObject* newGameObject){
    if(FindNode(newGameObject) || !newGameObject) return;
    
    uint32_t id = nextId++;
    HierarchyNode node(newGameObject);
    nodes.emplace(id, node);
    objectToId[newGameObject] = id;
}
void HierarchySystem::AddCommunication(GameObject* object, std::vector<GameObject*> others, bool isChildren){
    if (!object) return;
    
    HierarchyNode* node = FindNode(object);
    if(!node){
        AddGameObject(object);
        node = FindNode(object); 
        if (!node) return; 
    }
    
    uint32_t objectId = objectToId[object];
    
    for(GameObject* other : others){
        if (!other || other == object) continue;
        
        HierarchyNode* otherNode = FindNode(other);
        if(!otherNode){
            AddGameObject(other);
            otherNode = FindNode(other);
            if(!otherNode) continue;
        }
        
        uint32_t otherId = objectToId[other];
        
        if(isChildren){
            AddUniqueId(node->childrenIds, otherId);    
            AddUniqueId(otherNode->parentsIds, objectId); 
        } 
        else{
            AddUniqueId(node->parentsIds, otherId);     
            AddUniqueId(otherNode->childrenIds, objectId);
        }
    }
}

void HierarchySystem::RemoveCommunication(GameObject* object, GameObject* other, bool isChildren){
    HierarchyNode* node = FindNode(object);
    HierarchyNode* otherNode = FindNode(other);
    if(!node || !otherNode) return;
    
    uint32_t objectId = objectToId[object];
    uint32_t otherId = objectToId[other];
    
    if (isChildren){
        auto& children = node->childrenIds;
        children.erase(std::remove(children.begin(), children.end(), otherId), children.end());
        
        auto& parents = otherNode->parentsIds;
        parents.erase(std::remove(parents.begin(), parents.end(), objectId), parents.end());
    }
    else{
        auto& parents = node->parentsIds;
        parents.erase(std::remove(parents.begin(), parents.end(), otherId), parents.end());
        
        auto& children = otherNode->childrenIds;
        children.erase(std::remove(children.begin(), children.end(), objectId), children.end());
    }
}

void HierarchySystem::AddUniqueId(std::vector<uint32_t>& vec, uint32_t id){
    if (std::find(vec.begin(), vec.end(), id) == vec.end()){
        vec.push_back(id);
    }
}

void HierarchySystem::RemoveGameObjectRecursive(GameObject* gameObjectToDelete, DeleteStrategy strategy) {
    HierarchyNode* node = FindNode(gameObjectToDelete);
    if(!node) return;
    
    uint32_t deletedId = objectToId[gameObjectToDelete];
    
    if(strategy == DeleteStrategy::CASCADE){
        for(uint32_t childId : node->childrenIds){
            auto childIt = nodes.find(childId);
            if(childIt != nodes.end()){
                RemoveGameObjectRecursive(childIt->second.object, strategy);
            }
        }
    } 
    else if(strategy == DeleteStrategy::REATTACH_TO_PARENT){
        for(uint32_t parentId : node->parentsIds){
            auto parentIt = nodes.find(parentId);
            if(parentIt != nodes.end()){
                parentIt->second.childrenIds.insert(parentIt->second.childrenIds.end(),
                                                    node->childrenIds.begin(),
                                                    node->childrenIds.end());
            }
        }
        
        for(uint32_t childId : node->childrenIds){
            auto childIt = nodes.find(childId);
            if(childIt != nodes.end()) {
                childIt->second.parentsIds.insert(childIt->second.parentsIds.end(),
                                                  node->parentsIds.begin(),
                                                  node->parentsIds.end());
                auto& parents = childIt->second.parentsIds;
                parents.erase(std::remove(parents.begin(), parents.end(), deletedId), 
                              parents.end());
            }
        }
    }
    
    for(uint32_t parentId : node->parentsIds){
        auto parentIt = nodes.find(parentId);
        if(parentIt != nodes.end()) {
            auto& children = parentIt->second.childrenIds;
            children.erase(std::remove(children.begin(), children.end(), deletedId), 
                           children.end());
        }
    }
    
    nodes.erase(deletedId);
    objectToId.erase(gameObjectToDelete);
}

void HierarchySystem::RemoveGameObject(GameObject* gameObjectToDelete, DeleteStrategy strategy) {
    HierarchyNode* node = FindNode(gameObjectToDelete);
    if(!node) return;
    
    uint32_t deletedId = objectToId[gameObjectToDelete];
    
    if (strategy == DeleteStrategy::CASCADE) {
        for (uint32_t childId : node->childrenIds) {
            auto childIt = nodes.find(childId);
            if (childIt != nodes.end()) {
                RemoveGameObjectRecursive(childIt->second.object, strategy);
            }
        }
    } 
    else if (strategy == DeleteStrategy::REATTACH_TO_PARENT) {
        for (uint32_t parentId : node->parentsIds) {
            auto parentIt = nodes.find(parentId);
            if (parentIt != nodes.end()) {
                parentIt->second.childrenIds.insert(parentIt->second.childrenIds.end(),
                                                    node->childrenIds.begin(),
                                                    node->childrenIds.end());
            }
        }
        
        for (uint32_t childId : node->childrenIds) {
            auto childIt = nodes.find(childId);
            if (childIt != nodes.end()) {
                childIt->second.parentsIds.insert(childIt->second.parentsIds.end(),
                                                    node->parentsIds.begin(),
                                                    node->parentsIds.end());
                auto& parents = childIt->second.parentsIds;
                parents.erase(std::remove(parents.begin(), parents.end(), deletedId), 
                                parents.end());
            }
        }
    }
    
    for (uint32_t parentId : node->parentsIds) {
        auto parentIt = nodes.find(parentId);
        if (parentIt != nodes.end()) {
            auto& children = parentIt->second.childrenIds;
            children.erase(std::remove(children.begin(), children.end(), deletedId), 
                            children.end());
        }
    }
    
    nodes.erase(deletedId);
    objectToId.erase(gameObjectToDelete);
}

GameObject* HierarchySystem::GetParent(GameObject* gameObject, int index){
    HierarchyNode* node = FindNode(gameObject);
    if(!node){
        AddGameObject(gameObject); 
        return nullptr;
    }

    if (index < 0) return nullptr;
    if (node->parentsIds.empty()) return nullptr;
    if (index < node->parentsIds.size()) {
        return nodes[node->parentsIds[index]].object;
    }
    return nullptr;
}
std::vector<GameObject*> HierarchySystem::GetParents(GameObject* gameObject){
    HierarchyNode* node = FindNode(gameObject);
    if(!node){
        AddGameObject(gameObject); 
        return {};
    }

    return GetObjectsByIds(node->parentsIds);
}

GameObject* HierarchySystem::GetChild(GameObject* gameObject, int index){
    HierarchyNode* node = FindNode(gameObject);
    if(!node){
        AddGameObject(gameObject); 
        return nullptr;
    }

    if (index < 0) return nullptr;
    if (node->childrenIds.empty()) return nullptr;
    if (index < node->childrenIds.size()) {
        return nodes[node->childrenIds[index]].object;
    }
    return nullptr;
}
std::vector<GameObject*> HierarchySystem::GetChilds(GameObject* gameObject){
    HierarchyNode* node = FindNode(gameObject);
    if(!node){
        AddGameObject(gameObject); 
        return {};
    }

    return GetObjectsByIds(node->childrenIds);
}

std::unordered_map<int, std::vector<GameObject*>> HierarchySystem::GetParentTree(GameObject* gameObject, int layers){
    HierarchyNode* node = FindNode(gameObject);
    if(!node){
        AddGameObject(gameObject); 
        return std::unordered_map<int, std::vector<GameObject*>>();
    }

    std::unordered_map<int, std::vector<GameObject*>> tree;

    std::vector<GameObject*> oldestParents = GetParents(gameObject);
    tree.emplace(0, oldestParents);

    for(int layer = 0; layer < layers; layer++){
        oldestParents = tree[layer];

        std::vector<GameObject*> newLayerObject;
        for(auto* oldestParent : oldestParents){
            std::vector<GameObject*> currentParents = GetParents(oldestParent);
            newLayerObject.insert(newLayerObject.end(), 
                    std::make_move_iterator(currentParents.begin()),
                    std::make_move_iterator(currentParents.end()));

            currentParents.clear();
        }
        tree.emplace(layer + 1, newLayerObject);
        newLayerObject.clear();
    }
    return tree;
}
std::unordered_map<int, std::vector<GameObject*>> HierarchySystem::GetChildrenTree(GameObject* gameObject, int layers){
    HierarchyNode* node = FindNode(gameObject);
    if(!node){
        AddGameObject(gameObject); 
        return std::unordered_map<int, std::vector<GameObject*>>();
    }

    std::unordered_map<int, std::vector<GameObject*>> tree;

    std::vector<GameObject*> yengestChilds = GetChilds(gameObject);
    tree.emplace(0, yengestChilds);

    for(int layer = 0; layer < layers; layer++){
        yengestChilds = tree[layer];

        std::vector<GameObject*> newLayerObject;
        for(auto* yangChild : yengestChilds){
            std::vector<GameObject*> currentChilds = GetChilds(yangChild);
            newLayerObject.insert(newLayerObject.end(), 
                    std::make_move_iterator(currentChilds.begin()),
                    std::make_move_iterator(currentChilds.end()));

            currentChilds.clear();
        }
        tree.emplace(layer + 1, newLayerObject);
        newLayerObject.clear();
    }
    return tree;
}