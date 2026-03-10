#pragma once

#include "Component.h"
#include <vector>
#include <unordered_map>
#include <string>

class CollisionEvent;
class ColliderManager;
class PhysicalBody;

class BaseCollider : public Component{
protected:
    ColliderManager* colliderManager{nullptr};
    std::unordered_map<BaseCollider*, const CollisionEvent*> currentCollisions;
    bool m_isBeingDestroyed{false}; 

    bool m_isTrigger{false};
    bool m_isEnabled{true};
    std::string m_tag;

public:
    BaseCollider() = default;
    virtual ~BaseCollider() = default; 

    void SetColliderManager(ColliderManager* manager) { colliderManager = manager; }
    ColliderManager* GetColliderManager() const { return colliderManager; }

    void SetIsTrigger(bool isTrigger) { m_isTrigger = isTrigger; }
    bool IsTrigger() const { return m_isTrigger; }

    void SetEnabled(bool enabled) { m_isEnabled = enabled; }
    bool IsEnabled() const { return m_isEnabled; }

    void SetTag(const std::string& tag) { m_tag = tag; }
    const std::string& GetTag() const { return m_tag; }

    std::unordered_map<BaseCollider*, const CollisionEvent*>& GetCurrentCollisions() { 
        return currentCollisions; 
    }
    const auto& GetCurrentCollisions() const { return currentCollisions; }
    
    bool IsCollidingWith(const BaseCollider* other) const { 
        return currentCollisions.find(const_cast<BaseCollider*>(other)) != currentCollisions.end();
    }
    
    void _AddCollision(BaseCollider* other, const CollisionEvent* event) {
        currentCollisions[other] = event;
    }
    
    void _RemoveCollision(BaseCollider* other) {
        currentCollisions.erase(other);
    }
    
    void _ClearCollisions() {
        currentCollisions.clear();
    }

    virtual bool Intersects(const BaseCollider* other) const = 0;

    virtual void OnCollisionEnter(const class CollisionEvent& event) {}
    virtual void OnCollisionStay(const class CollisionEvent& event) {}
    virtual void OnCollisionExit(const class CollisionEvent& event) {}
    
    virtual void OnTriggerEnter(const class CollisionEvent& event) {}
    virtual void OnTriggerStay(const class CollisionEvent& event) {}
    virtual void OnTriggerExit(const class CollisionEvent& event) {}

    virtual float GetVolume() const = 0; 

    void Destroy() override;

    PhysicalBody* GetPhysicalBody() const;
};