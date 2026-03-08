#include "TagManager.h"

#include "Tag.h" 
#include "TagLibrary.h"
#include "GameObject.h"
#include <vector>
#include <unordered_map>

TagManager* TagManager::instance = nullptr;

TagManager& TagManager::GetInstance() {
    if (!instance) {
        instance = new TagManager();
    }
    return *instance;
}

void TagManager::RegisterObject(GameObject* obj, const TagLibrary& tag, bool unreggisstred = false){
    if(unreggisstred) UnregisterObject(obj);
    
    taggedObjects[tag].push_back(obj);

    Tag* existingTag = obj->GetComponentOfType<Tag>();
    if (existingTag) {
        existingTag->SetTag(tag);
    } 
    else {
        Tag* tagComponent = new Tag(tag);
        obj->AddComponent(tagComponent);
    }
}

void TagManager::UnregisterObject(GameObject* obj) {
    for (auto& [tag, objects] : taggedObjects) {
        auto it = std::find(objects.begin(), objects.end(), obj);
        if (it != objects.end()) {
            objects.erase(it);
            if (objects.empty()) {
                auto tagToRemove = tag;
            }
        }
    }
}

void TagManager::ChangeTag(GameObject* obj, const TagLibrary& newTag){
    UnregisterObject(obj);
    RegisterObject(obj, newTag);
}

std::vector<GameObject*> TagManager::FindObjectsWithTag(const TagLibrary& tag){
    auto it = taggedObjects.find(tag);
    if (it != taggedObjects.end()) {
        return it->second;
    }
    return {}; 
}

std::vector<TagLibrary> TagManager::GetAllTags() const{
    std::vector<TagLibrary> tags;
    tags.reserve(taggedObjects.size());
    
    for (const auto& [tag, objects] : taggedObjects) {
        if (!objects.empty()) {
            tags.push_back(tag);
        }
    }
    
    return tags;
}

void TagManager::Clear() {
    taggedObjects.clear();
}

void TagManager::DestroyInstance() {
    delete instance;
    instance = nullptr;
}