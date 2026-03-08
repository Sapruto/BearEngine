#pragma once

#include "PhysicFeature.h"
#include "ImpulsePhysicsData.h"
#include "Vector3.h"
#include "PhysicsEvents.h"
#include "Force.h"
#include <vector>
#include <memory>

class ImpulseModule : public PhysicFeature{
private:
    Force main_force;

    Vector3 velocity;

public:
    ImpulseModule();
    ~ImpulseModule();

    void AddForce(Vector3 direction, float magnitude);
    void AddForce(const Force& new_force);

    void Initialize() override {  }

    void ChangeBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;

    const Vector3& GetVelocity() const { return velocity; }  
    const Force& GetForce() const { return main_force; }  
};