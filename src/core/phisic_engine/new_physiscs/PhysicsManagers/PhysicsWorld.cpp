#include "PhysicsWorld.h"

#include "PhysicFeature.h"
#include "PhysicalBody.h"

#include "ColliderManager.h"
#include "CollisionEvent.h"
#include "PhysicsCollisionData.h"
#include "IntersectionSegments3D.h"

void PhysicsWorld::UnsubscribeAll(PhysicalBody* body) {
    for (auto& [type, vec] : subscribers) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [body](PhysicFeature* feature) {
                    return feature->GetBody() == body;
                }),
            vec.end()
        );
    }
}

void PhysicsWorld::AddBody(PhysicalBody* body) {
    if (!body) return;
    body->SetPhysicWorld(this);
    bodies.push_back(body);

    body->Initialize();
}

void PhysicsWorld::RemoveBody(PhysicalBody* body){
    auto it = std::find(bodies.begin(), bodies.end(), body);
    
    if (it != bodies.end()) {
        (*it)->ClearWorld();    
        UnsubscribeAll(*it);  
        bodies.erase(it);
    }
}

void PhysicsWorld::Start(){
    for(auto* manager : physicsManagers){
        manager->Start();
    }

    for(auto* body : bodies){
        body->SetPhysicWorld(this);
        body->Initialize();
    }

    isStarted = true;
}

void PhysicsWorld::Update(){
    for(auto* manager : physicsManagers){
        manager->Update();
    }

    for(auto* body : bodies){
        body->PhysicsUpdate();
    }
    
    if (colliderManager) {
        colliderManager->CheckCollisions();
        
        for (auto& collisionEvent : colliderManager->TakeCollisionEvents()) {
            if (!collisionEvent) continue;
            
            BasePhysicsEvent event(
                collisionEvent.get(),     
                PhysicEventType::CollisionEvent 
            );
            
            if (auto it = subscribers.find(PhysicEventType::CollisionEvent); 
                it != subscribers.end()) {
                auto subscribers_copy = it->second;
                for (auto* feature : subscribers_copy) {
                    feature->ReactionOnEvent(&event);
                }
            }
        }
    }
}

void PhysicsWorld::CreateEvent(BasePhysicsEvent* physicsEvent){
    if (!physicsEvent) return;
    
    PhysicEventType typeEvent = physicsEvent->GetType();
    
    if (auto it = subscribers.find(typeEvent); it != subscribers.end()) {
        auto subscribers_copy = it->second;
        for (auto* feature : subscribers_copy) {
            feature->ReactionOnEvent(physicsEvent);
        }
    }
}