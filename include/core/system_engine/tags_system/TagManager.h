#pragma once

#include "TagLibrary.h"
#include "GameObject.h"
#include <vector>
#include <unordered_map>

class TagManager {
private:
    static TagManager* instance;

    std::unordered_map<TagLibrary, std::vector<GameObject*>, TagLibrary::Hash> taggedObjects;
    
public:
    static TagManager& GetInstance();

    void RegisterObject(GameObject* obj, const TagLibrary& tag, bool unreggisstred);
    
    void UnregisterObject(GameObject* obj);
    
    void ChangeTag(GameObject* obj, const TagLibrary& newTag);
    
    std::vector<GameObject*> FindObjectsWithTag(const TagLibrary& tag);
    
    std::vector<TagLibrary> GetAllTags() const;
    
    void Clear();

    static void DestroyInstance();
    
private:
    TagManager() = default;
    ~TagManager() = default;
};