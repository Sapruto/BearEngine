#pragma once

#include "LocalPhysicCollisionSystem.h"

#include "BasePhysicManager.h"
#include "PhysicalBody.h"

class PhysicCollisionSystem : public BasePhysicManager{
private:
    std::vector<LocalPhysicCollisionSystem> bodySystems;

    std::unordered_map<LocalPhysicCollisionSystem*, std::vector<PhysicalBody*>> systemToBody;
    std::unordered_map<PhysicalBody*, LocalPhysicCollisionSystem*> bodyToSystem;

public:
    PhysicCollisionSystem() = default;

    void Start() override;
    void Update() override;
    void Destroy() override;

    void AddSystem(const std::vector<PhysicalBody*>& bodies, float threshold); 
    void AddBodyToSystem(PhysicalBody* body, LocalPhysicCollisionSystem* localSystem);

    void EnsureSameSystem(PhysicalBody* bodyA, PhysicalBody* bodyB, float threshold = 2.0f);
    void CombineSystems(LocalPhysicCollisionSystem* localSystemA, LocalPhysicCollisionSystem* localSystemB);

    LocalPhysicCollisionSystem* GetSystem(PhysicalBody* body);

    void RemoveSystem(LocalPhysicCollisionSystem* localSystem);

    std::vector<PhysicalBody*> GetBodyOnSystem(LocalPhysicCollisionSystem* localSystem);

    int GetSystemCount() const { return bodySystems.size(); }
};