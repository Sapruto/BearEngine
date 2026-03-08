#pragma once

#include "Component.h"
#include <vector>
#include <unordered_map>
#include <string>
#include "BaseCollider.h"

class CollisionEvent;
class ColliderManager;

template<typename ITransform, typename IVector>
class Collider : public BaseCollider {
protected:
    const ITransform* transform{nullptr}; 

public:
    Collider() = default;
    virtual ~Collider() = default;

    virtual bool ContainsPoint(const IVector& point) const = 0;
    virtual IVector GetCenter() const = 0;
    
    virtual bool GetCollisionInfo(const BaseCollider* other, 
                                  IVector& point, 
                                  IVector& normal, 
                                  float& penetration) const {
        return false;
    }
    
    const ITransform* GetTransform() const { return transform; }
};