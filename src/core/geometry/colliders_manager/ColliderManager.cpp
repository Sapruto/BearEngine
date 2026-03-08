#include "ColliderManager.h"
#include <algorithm>

ColliderManager::~ColliderManager() {
    for (auto* collider : all_colliders) {
        if (collider) {
            collider->SetColliderManager(nullptr);
        }
    }
    all_colliders.clear();
    previousCollisions.clear();
    currentFrameEvents.clear();
}

void ColliderManager::AddCollider(BaseCollider* collider) {
    if (!collider) return;
    
    if (std::find(all_colliders.begin(), all_colliders.end(), collider) != all_colliders.end()) {
        return;
    }
    
    collider->SetColliderManager(this);
    all_colliders.push_back(collider);
}

void ColliderManager::CheckPair(BaseCollider* a, BaseCollider* b) {
    if (!a->IsEnabled() || !b->IsEnabled()) return;
    if (!a->Intersects(b)) return;
    
    auto eventForA = std::make_unique<CollisionEvent>(CollisionEvent::State::STAY, a, b);
    auto eventForB = std::make_unique<CollisionEvent>(CollisionEvent::State::STAY, b, a);
    
    CollisionEvent* rawEventA = eventForA.get();
    CollisionEvent* rawEventB = eventForB.get();
    
    a->_AddCollision(b, rawEventA);
    b->_AddCollision(a, rawEventB);
    
    currentFrameEvents.push_back(std::move(eventForA));
    currentFrameEvents.push_back(std::move(eventForB));
}

void ColliderManager::ProcessEvents() {
    for (auto* collider : all_colliders) {
        if (!collider) continue;
        
        auto& current = collider->GetCurrentCollisions();
        auto& previous = previousCollisions[collider];
        
        for (const auto& [other, event] : current) {
            if (!other) continue;
            
            auto prevIt = previous.find(other);
            if (prevIt == previous.end()) {
                CollisionEvent enterEvent(CollisionEvent::State::ENTER, collider, other);
                
                if (collider->IsTrigger()) {
                    collider->OnTriggerEnter(enterEvent);
                } else {
                    collider->OnCollisionEnter(enterEvent);
                }
                
                previous[other] = true;
            } else {
                if (event) { 
                    if (collider->IsTrigger()) {
                        collider->OnTriggerStay(*event);
                    } else {
                        collider->OnCollisionStay(*event);
                    }
                }
            }
        }
        
        for (auto it = previous.begin(); it != previous.end();) {
            if (current.find(it->first) == current.end()) {
                CollisionEvent exitEvent(CollisionEvent::State::EXIT, collider, it->first);
                
                if (collider->IsTrigger()) {
                    collider->OnTriggerExit(exitEvent);
                } else {
                    collider->OnCollisionExit(exitEvent);
                }
                
                it = previous.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void ColliderManager::RemoveCollider(BaseCollider* collider) {
    if (!collider) return;
    
    auto it = std::find(all_colliders.begin(), all_colliders.end(), collider);
    if (it != all_colliders.end()) {
        for (auto* other : all_colliders) {
            if (other && other != collider) {
                other->_RemoveCollision(collider);
            }
        }
        
        collider->_ClearCollisions();
        collider->SetColliderManager(nullptr);
        
        previousCollisions.erase(collider);
        all_colliders.erase(it);
    }
}

void ColliderManager::CheckCollisions() {
    currentFrameEvents.clear();
    
    for (auto* collider : all_colliders) {
        if (collider) {
            collider->_ClearCollisions();
        }
    }
    
    for (size_t i = 0; i < all_colliders.size(); ++i) {
        auto* a = all_colliders[i];
        if (!a || !a->IsEnabled()) continue;
        
        for (size_t j = i + 1; j < all_colliders.size(); ++j) {
            auto* b = all_colliders[j];
            if (!b || !b->IsEnabled()) continue;
            
            CheckPair(a, b);
        }
    }
    
    ProcessEvents();
}

std::vector<BaseCollider*> ColliderManager::GetCollisionsFor(BaseCollider* collider) const {
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

bool ColliderManager::AreColliding(const BaseCollider* a, const BaseCollider* b) const {
    return a && b && a->IsCollidingWith(b);
}