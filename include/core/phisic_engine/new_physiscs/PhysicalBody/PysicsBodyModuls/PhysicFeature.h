#pragma once

#include "PhysicsEvents.h"
#include <vector>

class PhysicalBody;

class PhysicFeature{
protected:
    PhysicalBody* body;

    std::vector<PhysicEventType> events_subscribed;

    float layer;

    void SetLayer(float layer){
        this->layer = layer;
    }

    void SubcribeEvent(PhysicEventType event_type){
        events_subscribed.push_back(event_type);
    }
public:
    PhysicFeature() = default;
    virtual ~PhysicFeature() = default;

    void SetBody(PhysicalBody* body){
        this->body = body;
    }

    virtual void Initialize() = 0;
    
    virtual void ChangeBody() = 0;
    virtual void ReactionOnEvent(BasePhysicsEvent* event) = 0;

    virtual void Destroy() = 0;

    std::vector<PhysicEventType> GetSubscribedEvents(){
        return events_subscribed;
    }

    float GetLayer(){ return layer; }
};