#include "PhysicCollisionSystem.h"

void PhysicCollisionSystem::Start(){

}
void PhysicCollisionSystem::Update(){
    for(auto& system : bodySystems){
        system.UpdateLocalSystem();
    }
}
void PhysicCollisionSystem::Destroy(){
    for(auto& system : bodySystems){
        system.DestroyLocalSystem();
    }
}

void PhysicCollisionSystem::AddSystem(const std::vector<PhysicalBody*>& bodies, float threshold) {
    LocalPhysicCollisionSystem newSystem(this, threshold);
    bodySystems.push_back(newSystem);
    
    LocalPhysicCollisionSystem* ptr = &bodySystems.back();
    
    for(auto* body : bodies){
        AddBodyToSystem(body, ptr);
    }
}

void PhysicCollisionSystem::AddBodyToSystem(PhysicalBody* body, LocalPhysicCollisionSystem* localSystem){
    LocalPhysicCollisionSystem* currentSystem = GetSystem(body);
    
    if(currentSystem){
        if(currentSystem != localSystem){
            CombineSystems(currentSystem, localSystem);
        }
    } 
    else{
        auto it = systemToBody.find(localSystem);
        if(it != systemToBody.end()) {
            it->second.push_back(body);
            bodyToSystem[body] = localSystem;
        }
    }
}

void PhysicCollisionSystem::EnsureSameSystem(PhysicalBody* bodyA, PhysicalBody* bodyB, float threshold) {
    LocalPhysicCollisionSystem* systemA = GetSystem(bodyA);
    LocalPhysicCollisionSystem* systemB = GetSystem(bodyB);
    
    if (!systemA && !systemB){
        AddSystem({bodyA, bodyB}, threshold);
    }
    else if(systemA && !systemB){
        AddBodyToSystem(bodyB, systemA);
    }
    else if(!systemA && systemB){
        AddBodyToSystem(bodyA, systemB);
    }
    else if(systemA != systemB){
        CombineSystems(systemA, systemB);
    }
}

void PhysicCollisionSystem::CombineSystems(LocalPhysicCollisionSystem* localSystemA, LocalPhysicCollisionSystem* localSystemB){
    if (localSystemA == localSystemB) return;
    
    std::vector<PhysicalBody*> bodiesFromB = GetBodyOnSystem(localSystemB);
    
    auto itA = systemToBody.find(localSystemA);
    if(itA != systemToBody.end()){
        for(PhysicalBody* body : bodiesFromB){
            itA->second.push_back(body);
            bodyToSystem[body] = localSystemA;
        }
        
        float newThreshold = std::min(localSystemA->GetThreshold(), localSystemB->GetThreshold());
        localSystemA->SetThreshold(newThreshold);
    }
    
    RemoveSystem(localSystemB);
}

LocalPhysicCollisionSystem* PhysicCollisionSystem::GetSystem(PhysicalBody* body){
    auto it = bodyToSystem.find(body);
    if (it != bodyToSystem.end()) {
        return it->second;
    }
    return nullptr;
}

void PhysicCollisionSystem::RemoveSystem(LocalPhysicCollisionSystem* localSystem){
    auto bodiesIt = systemToBody.find(localSystem);
    if (bodiesIt != systemToBody.end()) {
        for (PhysicalBody* body : bodiesIt->second) {
            bodyToSystem.erase(body);
        }
        systemToBody.erase(bodiesIt);
    }
    
    auto vecIt = std::find_if(bodySystems.begin(), bodySystems.end(),
        [localSystem](const LocalPhysicCollisionSystem& sys) {
            return &sys == localSystem;
        });
    
    if (vecIt != bodySystems.end()) {
        bodySystems.erase(vecIt);
    }
}

std::vector<PhysicalBody*> PhysicCollisionSystem::GetBodyOnSystem(LocalPhysicCollisionSystem* localSystem){
    auto it = systemToBody.find(localSystem);
    if (it != systemToBody.end()) {
        return it->second;
    }
    return {};
}