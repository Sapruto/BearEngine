#include "PhysicReactionOnCollisions.h"

#include "PhysicalBody.h"
#include "CollisionEvent.h"

PhysicReactionOnCollisions::PhysicReactionOnCollisions(){
    SetLayer(10);
    SubcribeEvent(PhysicEventType::CollisionEvent);
}

void PhysicReactionOnCollisions::Initialize(){
    isValid = true;
    impulseModule = body->GetFeatureOfType<ImpulseModule>();
    float mass = body->GetMass();

    if(!impulseModule || mass == 0){
        isValid = false;
        return;
    }
}

void PhysicReactionOnCollisions::UpdateBody(){

}

void PhysicReactionOnCollisions::ReactionOnEvent(BasePhysicsEvent* event){
    if(!isValid || !impulseModule || !system) return;

    CollisionEvent* collisionEvent = dynamic_cast<CollisionEvent*>(event->GetData());
    if(!collisionEvent) return;
    
    system->EnsureSameSystem(body, collisionEvent->GetOther()->gameObject->GetComponentOfType<PhysicalBody>(), treshold);
}

void PhysicReactionOnCollisions::Destroy() {
    impulseModule = nullptr;
    isValid = false;
}