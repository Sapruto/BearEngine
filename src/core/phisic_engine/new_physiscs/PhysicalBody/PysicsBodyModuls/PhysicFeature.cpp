#include "PhysicFeature.h"

#include "PhysicsWorld.h"
#include "PhysicalBody.h"

void PhysicFeature::FeatureInitialize(){
    if(!body) return;

    const PhysicsWorld* world = body->GetWorld();
    if(!world) return;

    for(auto& event : events_subscribed){
        const_cast<PhysicsWorld*>(world)->Subscribe(this, event);
    }
}

void PhysicFeature::FeatureDestroy(){
    /*if(!body) return;

    const PhysicsWorld* world = body->GetWorld();
    if(!world) return;

    for(auto& event : events_subscribed){
        const_cast<PhysicsWorld*>(world)->Unsubscribe(this, event);
    }*/
}