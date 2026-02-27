#pragma once

#include <vector>
#include <memory>  
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "include/core/math/geometry/colliders_manager/CollisionEvent.h"
#include "include/core/math/geometry/colliders/Collider.h"

class ColliderManager {
private:
    std::vector<Collider*> all_colliders;
    
    std::unordered_map<Collider*, std::unordered_map<Collider*, bool>> previousCollisions;
    
    std::vector<std::unique_ptr<CollisionEvent>> currentFrameEvents;

    void CheckPair(Collider* a, Collider* b);
    
    void ProcessEvents();

public:
    ColliderManager() = default;
    ~ColliderManager();

    void AddCollider(Collider* collider);
    
    void RemoveCollider(Collider* collider);

    void CheckCollisions();
    
    std::vector<Collider*> GetCollisionsFor(Collider* collider) const;
    
    bool AreColliding(const Collider* a, const Collider* b) const; 
};