#pragma once

#include "PhysicsEvents.h"
#include <vector>
#include <algorithm> 
#include <cstddef>

class PhysicalBody;
class PhysicsWorld;

class PhysicFeature{
protected:
    PhysicalBody* body;

    std::vector<PhysicEventType> events_subscribed;

    int layer = 0; 

    void SetLayer(int layer){
        this->layer = layer;
    }

    void SubcribeEvent(PhysicEventType event_type){
        if (std::find(events_subscribed.begin(), events_subscribed.end(), event_type) 
            == events_subscribed.end()) {
            events_subscribed.push_back(event_type);
        }
    }
public:
    PhysicFeature() = default;
    virtual ~PhysicFeature() = default;

    void SetBody(PhysicalBody* body){
        this->body = body;
    }

    void FeatureInitialize();

    virtual void Initialize() = 0;
    
    virtual void UpdateBody() = 0;
    virtual void ReactionOnEvent(BasePhysicsEvent* event) = 0;

    virtual void Destroy() = 0;

    void FeatureDestroy();

    std::vector<PhysicEventType> GetSubscribedEvents(){
        return events_subscribed;
    }

    PhysicalBody* GetBody() const { return body; }
    int GetLayer() const { return layer; } 
};