#pragma once

#include "PhysicFeature.h"
#include "ImpulsePhysicsData.h"
#include "Vector3.h"
#include "PhysicsEvents.h"
#include "Force.h"
#include <vector>
#include <memory>

enum class ImpulseModuleMode {
    REALISTIC,  
    ARCADY     
};

class ImpulseModule : public PhysicFeature{
private:
    Force main_force;

    Vector3 velocity;

    ImpulseModuleMode mode;
    float damping;
    float maxSpeed;
    float worldLimit;

    void ApplyArcadyChange();

public:
    ImpulseModule();
    ~ImpulseModule();

    void SetMode(ImpulseModuleMode newMode){  mode = newMode; }

    void AddForce(Vector3 direction, float magnitude);
    void AddForce(const Force& new_force);

    void ReForce(const Force& new_force);

    void Initialize() override {  }

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;
    
    void SetVelocity(const Vector3& newVel) { velocity = newVel; }
    void SetDamping(float newDamping) { damping = newDamping; }
    void SetMaxSpeed(float newMaxSpeed) { maxSpeed = newMaxSpeed; }
    void SetWorldLimit(float newWorldLimit) { worldLimit = newWorldLimit; }

    const Vector3& GetVelocity() const { return velocity; }  
    Vector3 GetVelocity() { return velocity; }  
    const Force& GetForce() const { return main_force; }  
};