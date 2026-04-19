#pragma once

#include "PhysicFeature.h"
#include "Newtonian.h"

class Gravity : public PhysicFeature{
private:
    ImpulseModule* impulseModule = nullptr;
    Force gravitationForce;

    float gravitationScale = 9.8f;

    bool isValid = false;
public:
    Gravity();

    void SetGravitation(float scale) { gravitationScale = scale; }

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;

    bool Validate() { return isValid; }

    const Force& GetGarvitationForce() const { return gravitationForce; }
    float GetGravityScale() { return gravitationScale; }
};