#pragma once

#include "PhysicFeature.h"

#include "Force.h"

class ImpulseModule;
class Gravity;

class ArchimedForce : public PhysicFeature{
private:
    ImpulseModule* impulseModule = nullptr;
    Gravity* gravityModule = nullptr;

    bool isValid = false;

public:
    ArchimedForce();
    ~ArchimedForce(){}

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;
};