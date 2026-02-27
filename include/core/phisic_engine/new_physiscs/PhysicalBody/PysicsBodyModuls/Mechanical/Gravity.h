#pragma once

#include "include/core/phisic_engine/new_physiscs/PhysicalBody/PysicsBodyModuls/PhysicFeature.h"
#include "include/core/system_engine/time_system/Time.h"

class Gravity : public PhysicFeature{
private:
    float gravitationScale;
    float acceleration;
public:
    void SetGravitation(float newGravitationScale) { gravitationScale = newGravitationScale }

    void ChangeBody() override;
    void ReactionOnEvent() override;
    void GetStateFeature() override;
};