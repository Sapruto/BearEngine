#pragma once

#include "PhysicFeature.h"
#include "Newtonian.h"
#include "PhysicCollisionSystem.h"
#include "PhysicsEvents.h"

class PhysicReactionOnCollisions : public PhysicFeature{
private:
    ImpulseModule* impulseModule = nullptr;
    PhysicCollisionSystem* system = nullptr;

    float treshold{2.0f};

    bool isValid = false;
public:
    PhysicReactionOnCollisions();

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;

    bool Validate() { return isValid; }

    void SetSystem(PhysicCollisionSystem* system) { this->system = system; }
    void SetTreshold(float treshold) { this->treshold = treshold; }

    PhysicCollisionSystem* GetSystem() { return system; }
    float GetTreshold() { return treshold; }
};