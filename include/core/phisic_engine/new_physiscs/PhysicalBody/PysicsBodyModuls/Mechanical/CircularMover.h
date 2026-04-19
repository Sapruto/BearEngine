#pragma once

#include "PhysicFeature.h"
#include "PhysicalBody.h" 

#include "Vector3.h"

class ImpulseModule;

class CircularMover : public PhysicFeature{
private:
    ImpulseModule* impulseModule = nullptr;

    Vector3 anchorPoint;
    Vector3 direction;

    float radius;
    float periudRotate;

    float stiffness = 100.0f;
    float accelerationFactor = 10.0f; 
    
    bool isValid = false;

public:
    CircularMover(const Vector3& anchorPoint, 
    const Vector3& direction, 
    float radius,
    float periudRotate,
    float stiffness = 100.0f,
    float accelerationFactor = 10.0f);
    ~CircularMover() = default;

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override {  }

    void Destroy() override {  }

    void SetAnchorPoint(const Vector3& anchorPoint) { this->anchorPoint = anchorPoint; }
    void SetDirection(const Vector3& direction) { this->direction = direction; }
    void SetRadius(float radius) { this->radius = radius; }
    void SetPeriudRotate(float periudRotate) { this->periudRotate = periudRotate; }
    void SetStiffness(float stiffness) { this->stiffness = stiffness; }
    void SetAccelerationFactor(float accelerationFactor) { this->accelerationFactor = accelerationFactor; }
};