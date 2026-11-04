#pragma once

#include <unordered_map>
#include "BaseCollider.h"

class CollisionEvent;
class CollisionDomain;

template<typename ITransform, typename IVector>
class Collider : public BaseCollider {
protected:
    const ITransform* transform{nullptr}; 

    std::unordered_map<BaseCollider*, IVector> axisOverlap;

public:
    Collider() = default;
    virtual ~Collider() = default;

    virtual bool ContainsPoint(const IVector& point) const = 0;
    virtual IVector GetCenter() const = 0;
    
    virtual bool GetCollisionInfo([[maybe_unused]] const BaseCollider* other, 
                                    [[maybe_unused]] IVector& point, 
                                    [[maybe_unused]] IVector& normal, 
                                    [[maybe_unused]] float& penetration) const {
        return false;
    }
    
    const ITransform* GetTransform() const { return transform; }

    const IVector* GetOverlapAxis(const BaseCollider* other) const {
        auto it = axisOverlap.find(const_cast<BaseCollider*>(other));
        if (it != axisOverlap.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    bool IsOverlapping(const BaseCollider* other) const{
        return axisOverlap.find(const_cast<BaseCollider*>(other)) != axisOverlap.end();
    }

    void AddOverlap(BaseCollider* other, const IVector& axis){
        axisOverlap[other] = axis;
    }
};