#pragma once

#include <vector>
#include <memory>  
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "CollisionEvent.h"
#include "BaseCollider.h"

class CollisionDomain {
private:
    std::vector<BaseCollider*> colliders;
    
    std::unordered_map<BaseCollider*, std::unordered_map<BaseCollider*, bool>> previousCollisions;
    
    std::vector<std::unique_ptr<CollisionEvent>> currentFrameEvents;

    void CheckPair(BaseCollider* a, BaseCollider* b);

public:
    CollisionDomain() = default;
    ~CollisionDomain();

    void Initialize();

    void Update();

    void ProcessEvents();

    void AddCollider(BaseCollider* collider);
    
    void RemoveCollider(BaseCollider* collider);

    void CheckCollisions();
    
    std::vector<BaseCollider*> GetCollisionsFor(BaseCollider* collider) const;
    
    bool AreColliding(const BaseCollider* a, const BaseCollider* b) const; 

    std::vector<std::unique_ptr<CollisionEvent>> TakeCollisionEvents() {
        return std::move(currentFrameEvents);
    }
    
    const std::vector<std::unique_ptr<CollisionEvent>>& GetCollisionEvents() const {
        return currentFrameEvents;
    }
};