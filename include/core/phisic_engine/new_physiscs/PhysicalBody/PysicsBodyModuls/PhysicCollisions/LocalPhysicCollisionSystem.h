#pragma once

#include "Vector3.h"
#include "PhysicalBody.h"

class PhysicCollisionSystem;
class ImpulseModule;

class LocalPhysicCollisionSystem{
private:
    int attempts{0};
    int maxAttemps{10};

    Vector3 overallMomentum;
    float threshold;

    bool isActive;

    PhysicCollisionSystem* system;

    void CalculateImpulse(ImpulseModule* A, ImpulseModule* B, const Vector3& normal);
public:
    LocalPhysicCollisionSystem() = default;
    LocalPhysicCollisionSystem(PhysicCollisionSystem* system, float threshold=2) : system(system), threshold(threshold)  {}

    void UpdateLocalSystem();
    void DestroyLocalSystem();

    void ApplyOtherFactors(float factor);

    void SetGlobalSystem(PhysicCollisionSystem* system) { this->system = system; }
    void SetActive(bool isActive) { this->isActive = isActive; }
    void SetThreshold(float threshold) { this->threshold = threshold; }

    PhysicCollisionSystem* GetSystem() { return system; }
    bool GetActive() { return isActive; }
    float GetThreshold() { return threshold; }
};