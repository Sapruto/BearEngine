#pragma once

#include "PhysicsEvents.h"
#include <vector>
#include "PhysicsWorld.h"
#include "PhysicalBody.h" 

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

    void FeatureInitialize(){
        if(!body) return;

        const PhysicsWorld* world = body->GetWorld();
        if(!world) return;

        for(auto& event : events_subscribed){
            const_cast<PhysicsWorld*>(world)->Subscribe(this, event);
        }
    }

    virtual void Initialize() = 0;
    
    virtual void ChangeBody() = 0;
    virtual void ReactionOnEvent(BasePhysicsEvent* event) = 0;

    virtual void Destroy() = 0;

    void FeatureDestroy(){
        if(!body) return;

        const PhysicsWorld* world = body->GetWorld();
        if(!world) return;

        for(auto& event : events_subscribed){
            const_cast<PhysicsWorld*>(world)->Unsubscribe(this, event);
        }
    }

    std::vector<PhysicEventType> GetSubscribedEvents(){
        return events_subscribed;
    }

    PhysicalBody* GetBody() const { return body; }
    int GetLayer() const { return layer; } 
};