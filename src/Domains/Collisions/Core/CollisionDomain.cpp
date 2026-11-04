#include "Domains/Collisions/Core/CollisionDomain.h"
#include <algorithm>

CollisionDomain::~CollisionDomain() {
    for (auto* collider : colliders) {
        if (collider) {
            collider->SetCollisionDomain(nullptr);
        }
    }
    colliders.clear();
    previousCollisions.clear();
    currentFrameEvents.clear();
}

void CollisionDomain::AddCollider(BaseCollider* collider) {
    if (!collider) return;
    
    if (std::find(colliders.begin(), colliders.end(), collider) != colliders.end()) {
        return;
    }
    
    collider->SetCollisionDomain(this);
    collider->Initialize();
    colliders.push_back(collider);
}

void CollisionDomain::CheckPair(BaseCollider* a, BaseCollider* b) {
    if (!a->IsEnabled() || !b->IsEnabled()) return;
    if (!a->Intersects(b)) return;
    
    auto eventForA = std::make_unique<CollisionEvent>(CollisionEvent::State::STAY, a, b);
    auto eventForB = std::make_unique<CollisionEvent>(CollisionEvent::State::STAY, b, a);
    
    CollisionEvent* rawEventA = eventForA.get();
    CollisionEvent* rawEventB = eventForB.get();
    
    a->AddCollision(b, rawEventA);
    b->AddCollision(a, rawEventB);
    
    currentFrameEvents.push_back(std::move(eventForA));
    currentFrameEvents.push_back(std::move(eventForB));
}

void CollisionDomain::Initialize() {
    for(auto* collider : colliders){
        if (!collider) continue;
        collider->Initialize();
    }
}

void CollisionDomain::Update(){
    CheckCollisions();
    ProcessEvents();
}

void CollisionDomain::ProcessEvents() {
    for (auto* collider : colliders) {
        if (!collider) continue;
        
        auto& current = collider->GetCurrentCollisions();
        auto& previous = previousCollisions[collider];
        
        for (const auto& [other, event] : current) {
            if (!other) continue;
            
            auto prevIt = previous.find(other);
            if (prevIt == previous.end()) {
                auto enterEvent = std::make_unique<CollisionEvent>(CollisionEvent::State::ENTER, collider, other);
                CollisionEvent* rawEvent = enterEvent.get();
                
                const_cast<std::unordered_map<BaseCollider*, const CollisionEvent*>&>(current)[other] = rawEvent;
                
                if (collider->IsTrigger()) {
                    collider->OnTrigger(*rawEvent);
                } else {
                    collider->OnCollision(*rawEvent);
                }
                
                currentFrameEvents.push_back(std::move(enterEvent));
                previous[other] = true;
            } 
            else if (event) {
                if (collider->IsTrigger()) {
                    collider->OnTrigger(*event);
                } else {
                    collider->OnCollision(*event);
                }
            }
        }
        
        for (auto it = previous.begin(); it != previous.end();) {
            if (current.find(it->first) == current.end()) {
                auto exitEvent = std::make_unique<CollisionEvent>(CollisionEvent::State::EXIT, collider, it->first);
                CollisionEvent* rawEvent = exitEvent.get();
                
                if (collider->IsTrigger()) {
                    collider->OnTrigger(*rawEvent);
                } 
                else {
                    collider->OnCollision(*rawEvent);
                }
                
                currentFrameEvents.push_back(std::move(exitEvent));
                it = previous.erase(it);
            } 
            else {
                ++it;
            }
        }
    }
}

void CollisionDomain::RemoveCollider(BaseCollider* collider) {
    if (!collider) return;
    
    auto it = std::find(colliders.begin(), colliders.end(), collider);
    if (it != colliders.end()) {
        for (auto* other : colliders) {
            if (other && other != collider) {
                other->RemoveCollision(collider);
            }
        }
        
        collider->ClearCollisions();
        collider->SetCollisionDomain(nullptr);
        
        previousCollisions.erase(collider);
        colliders.erase(it);
    }
}

void CollisionDomain::CheckCollisions() {
    currentFrameEvents.clear();
    
    for (auto* collider : colliders) {
        if (collider) {
            collider->ClearCollisions();
        }
    }
    
    for (size_t i = 0; i < colliders.size(); ++i) {
        auto* a = colliders[i];
        if (!a || !a->IsEnabled()) continue;
        
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            auto* b = colliders[j];
            if (!b || !b->IsEnabled()) continue;
            
            CheckPair(a, b);
        }
    }
}

std::vector<BaseCollider*> CollisionDomain::GetCollisionsFor(BaseCollider* collider) const {
    std::vector<BaseCollider*> result;
    if (!collider) return result;
    
    const auto& collisions = collider->GetCurrentCollisions();
    result.reserve(collisions.size());
    
    for (const auto& [other, _] : collisions) {
        if (other) {
            result.push_back(other);
        }
    }
    return result;
}

bool CollisionDomain::AreColliding(const BaseCollider* a, const BaseCollider* b) const {
    return a && b && a->IsCollidingWith(b);
}