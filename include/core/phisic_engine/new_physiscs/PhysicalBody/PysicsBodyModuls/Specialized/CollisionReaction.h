#pragma once

#include "PhysicFeature.h"
#include "PhysicalBody.h"
#include "CollisionEvent.h"
#include "Newtonian.h"

class CollisionReaction : public PhysicFeature {
private:
    float restitution = 0.5f;
    float friction = 0.3f;    

    void HandleCollision(PhysicalBody* bodyA, PhysicalBody* bodyB,
                        ImpulseModule* impulseA, ImpulseModule* impulseB);
    
public:
    CollisionReaction();
    
    void Initialize() override {}
    void ChangeBody() override {}  
    void Destroy() override {}          
    
    void ReactionOnEvent(BasePhysicsEvent* event) override;
};