#include "LocalPhysicCollisionSystem.h"

#include "BaseCollider.h"
#include "CollisionEvent.h"
#include "Polyhedron3D.h"
#include "Transform3D.h"
#include "Newtonian.h"
#include "PhysicCollisionSystem.h"

#include <unordered_set> 
#include <utility>   
#include <functional>

void LocalPhysicCollisionSystem::CalculateImpulse(ImpulseModule* A, ImpulseModule* B, const Vector3& normal){
    float restitution = 1.0f; // А вот плохо так по нищему делать, надо сделать менеджер упругости и через него вот эту штуку считать ну это потом можно пока и так сойдет

    Vector3 velocityA = A->GetVelocity();
    Vector3 velocityB = B->GetVelocity();

    float velocityOnProjection = (velocityB - velocityA).dot(normal);

    if(velocityOnProjection >= 0) return;

    float scalar = -(1 + restitution) * velocityOnProjection / (1/A->GetBody()->GetMass() + 1/B->GetBody()->GetMass());
    Vector3 impulse = normal * scalar;

    A->SetVelocity(velocityA += (impulse / A->GetBody()->GetMass()));
    B->SetVelocity(velocityB -= (impulse / B->GetBody()->GetMass()));
}

void LocalPhysicCollisionSystem::UpdateLocalSystem(){
    if (attempts > maxAttemps) {
        attempts = 0;
        return;
    }
    
    std::vector<PhysicalBody*> bodis = system->GetBodyOnSystem(this);
    
    struct PairHash {
        std::size_t operator()(const std::pair<PhysicalBody*, PhysicalBody*>& p) const {
            auto h1 = std::hash<PhysicalBody*>{}(p.first);
            auto h2 = std::hash<PhysicalBody*>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
    
    std::unordered_set<std::pair<PhysicalBody*, PhysicalBody*>, PairHash> processedPairs;
    bool systemsMerged = false;
    
    for(auto* body : bodis){
        BaseCollider* collider = body->GetBaseCollider();
        Transform3D* transform = body->GetTransform();
        ImpulseModule* impulseModule = body->GetFeatureOfType<ImpulseModule>();

        Polyhedron3D* polyhedron = dynamic_cast<Polyhedron3D*>(collider);
        if(!polyhedron || !transform || !impulseModule) continue;

        auto& collisions = polyhedron->GetCurrentCollisions();
        for (const auto& [other, _] : collisions){
            PhysicalBody* otherBody = other->gameObject->GetComponentOfType<PhysicalBody>();
            if(!otherBody) continue;

            LocalPhysicCollisionSystem* otherSystem = system->GetSystem(otherBody);
            
            if(otherSystem != this){
                if(otherSystem){
                    system->CombineSystems(this, otherSystem);
                    systemsMerged = true;
                } 
                else
                {
                    system->AddBodyToSystem(otherBody, this);
                }
                continue;
            }
            
            auto [body1, body2] = std::minmax(body, otherBody);
            auto pair = std::make_pair(body1, body2);
            
            if (processedPairs.count(pair)) continue;
            processedPairs.insert(pair);
            
            ImpulseModule* otherImpulseModule = otherBody->GetFeatureOfType<ImpulseModule>();
            if (!otherImpulseModule) continue;
            
            const Vector3* penetration = polyhedron->GetOverlapAxis(other);
            if (!penetration) continue;
            
            Vector3 normal = penetration->normalized();
            CalculateImpulse(impulseModule, otherImpulseModule, normal);
        }
    }
    
    if(systemsMerged && attempts < maxAttemps){
        attempts++;
        UpdateLocalSystem();
        return;
    }
    
    attempts = 0;
}

void LocalPhysicCollisionSystem::DestroyLocalSystem(){

}

void LocalPhysicCollisionSystem::ApplyOtherFactors(float factor){
    
}