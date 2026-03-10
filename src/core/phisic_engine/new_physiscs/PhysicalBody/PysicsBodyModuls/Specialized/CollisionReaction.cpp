#include "CollisionReaction.h"

CollisionReaction::CollisionReaction(){
    SetLayer(100);
    SubcribeEvent(PhysicEventType::CollisionEvent);
}

void CollisionReaction::HandleCollision(PhysicalBody* bodyA, PhysicalBody* bodyB,
                    ImpulseModule* impulseA, ImpulseModule* impulseB){
    Vector3 dirAtoB = bodyB->GetTransform()->position - bodyA->GetTransform()->position;
    dirAtoB.normalize();
    
    float forceMagnitude = 10.0f;
    
    if (impulseA) {
        impulseA->AddForce(-dirAtoB, forceMagnitude);
    }
    
    if (impulseB) {
        impulseB->AddForce(dirAtoB, forceMagnitude);
    }
}

void CollisionReaction::ReactionOnEvent(BasePhysicsEvent* event){
    auto* collisionData = dynamic_cast<CollisionEvent*>(event->GetData());
    if (!collisionData) return;
    
    BaseCollider* selfCollider = collisionData->GetSelf();
    BaseCollider* otherCollider = collisionData->GetOther();
    
    PhysicalBody* selfBody = selfCollider->GetPhysicalBody();
    PhysicalBody* otherBody = otherCollider->GetPhysicalBody();
    
    if (!selfBody || !otherBody) return;
    
    auto* selfImpulse = selfBody->GetFeatureOfType<ImpulseModule>();
    auto* otherImpulse = otherBody->GetFeatureOfType<ImpulseModule>();
    
    HandleCollision(selfBody, otherBody, selfImpulse, otherImpulse);
}