#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map> 
#include "PhysicsEvents.h"

class PhysicalBody;
class ColliderManager;
class PhysicFeature;

class PhysicsWorld{
private:
    std::vector<PhysicalBody*> bodies;

    std::shared_ptr<ColliderManager> colliderManager;

    std::unordered_map<PhysicEventType, std::vector<PhysicFeature*>> subscribers;

    void UnsubscribeAll(PhysicalBody* body);

public:
    void SetColliderManager(std::shared_ptr<ColliderManager> colliderManager){ this->colliderManager = colliderManager; }

    void AddBody(PhysicalBody* body);
    
    void RemoveBody(PhysicalBody* body);

    void Start();
    void Update();

    void CreateEvent(BasePhysicsEvent* physicsEvent);

    void Subscribe(PhysicFeature* feature, PhysicEventType type) {
        if (!feature) return;
        subscribers[type].push_back(feature);
    }
    
    void Unsubscribe(PhysicFeature* feature, PhysicEventType type) {
        if (!feature) return;
        auto& vec = subscribers[type];
        vec.erase(std::remove(vec.begin(), vec.end(), feature), vec.end());
    }
    
    template<typename DataType>
    void DispatchEvent(PhysicEventType type, DataType&& data) {
        auto eventData = std::make_unique<DataType>(std::forward<DataType>(data));
        BasePhysicsEvent event(eventData.get(), type);
        
        if (auto it = subscribers.find(type); it != subscribers.end()) {
            auto subscribers_copy = it->second;
            for (auto* feature : subscribers_copy) {
                feature->ReactionOnEvent(&event);
            }
        }
    }

    const std::vector<PhysicalBody*>& GetAllBodies() const { return bodies; }
    int GetBodyCount() const { return bodies.size(); }
};
