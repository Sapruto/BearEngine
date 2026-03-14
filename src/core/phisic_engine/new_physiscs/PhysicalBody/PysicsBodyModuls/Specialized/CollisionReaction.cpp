#include "CollisionReaction.h"

#include "CollisionEvent.h"

CollisionReaction::CollisionReaction(){
    SetLayer(100);
    SubcribeEvent(PhysicEventType::CollisionEvent);
}

void CollisionReaction::HandleCollision(PhysicalBody* bodyA, PhysicalBody* bodyB,
                    ImpulseModule* impulseA, ImpulseModule* impulseB){
    
    Vector3 posA = bodyA->GetTransform()->position;
    Vector3 posB = bodyB->GetTransform()->position;
    
    Vector3 dirAtoB = posB - posA;
    float distance = dirAtoB.magnitude();
    
    if (distance < 0.001f) return; 
    
    dirAtoB = dirAtoB / distance;
    
    float radiusA = 0.5f; 
    float radiusB = 0.5f;
    float penetrationDepth = (radiusA + radiusB) - distance;
    
    if (penetrationDepth <= 0) return;
    

    float stiffness = 100.0f;
    
    float forceMagnitude = penetrationDepth * stiffness;
    
    float damping = 0.3f;
    
    if (impulseA) {
        float massA = bodyA->GetMass();
        impulseA->AddForce(-dirAtoB, forceMagnitude * massA * damping);
    }
    
    if (impulseB) {
        float massB = bodyB->GetMass();
        impulseB->AddForce(dirAtoB, forceMagnitude * massB * damping);
    }
}

void CollisionReaction::ReactionOnEvent(BasePhysicsEvent* event) {
    auto* collisionEvent = dynamic_cast<CollisionEvent*>(event->GetData());
    if (!collisionEvent) return;
    
    ImpulseModule* impulseModule = body->GetFeatureOfType<ImpulseModule>();
    if (impulseModule) {
        impulseModule->AddForce(Vector3(0, 10.0f, 0), 1.0f);
    }
}