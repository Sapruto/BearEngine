#pragma once

#include "include/core/math/Transform2D.h"
#include "include/core/math/Vector2.h"
#include "include/core/system_engine/component_system/Component.h"
#include <vector>
#include <unordered_map>
#include <string>

class CollisionEvent;
class ColliderManager;

class Collider : public Component {
private:
    ColliderManager* colliderManager{nullptr};
    std::unordered_map<Collider*, const CollisionEvent*> currentCollisions;
    bool m_isBeingDestroyed{false}; 

protected:
    bool m_isTrigger{false};
    bool m_isEnabled{true};
    std::string m_tag;
    const Transform2D* transform{nullptr};

public:
    Collider() = default;
    virtual ~Collider(); 
    
    Collider(const Collider&) = delete;
    Collider& operator=(const Collider&) = delete;
    
    Collider(Collider&& other) noexcept; 
    Collider& operator=(Collider&& other) noexcept;

    void SetColliderManager(ColliderManager* manager) { colliderManager = manager; }
    ColliderManager* GetColliderManager() const { return colliderManager; }

    void SetIsTrigger(bool isTrigger) { m_isTrigger = isTrigger; }
    bool IsTrigger() const { return m_isTrigger; }

    void SetEnabled(bool enabled) { m_isEnabled = enabled; }
    bool IsEnabled() const { return m_isEnabled; }

    void SetTag(const std::string& tag) { m_tag = tag; }
    const std::string& GetTag() const { return m_tag; }

    std::unordered_map<Collider*, const CollisionEvent*>& GetCurrentCollisions() { 
        return currentCollisions; 
    }
    const auto& GetCurrentCollisions() const { return currentCollisions; }
    
    bool IsCollidingWith(const Collider* other) const { 
        return currentCollisions.find(const_cast<Collider*>(other)) != currentCollisions.end();
    }
    
    void _AddCollision(Collider* other, const CollisionEvent* event) {
        currentCollisions[other] = event;
    }
    
    void _RemoveCollision(Collider* other) {
        currentCollisions.erase(other);
    }
    
    void _ClearCollisions() {
        currentCollisions.clear();
    }

    virtual bool ContainsPoint(const Vector2& point) const = 0;
    virtual bool Intersects(const Collider* other) const = 0;
    virtual Vector2 GetCenter() const = 0;
    
    virtual bool GetCollisionInfo(const Collider* other, 
                                  Vector2& point, 
                                  Vector2& normal, 
                                  float& penetration) const {
        return false;
    }

    virtual void OnCollisionEnter(const class CollisionEvent& event) {}
    virtual void OnCollisionStay(const class CollisionEvent& event) {}
    virtual void OnCollisionExit(const class CollisionEvent& event) {}
    
    virtual void OnTriggerEnter(const class CollisionEvent& event) {}
    virtual void OnTriggerStay(const class CollisionEvent& event) {}
    virtual void OnTriggerExit(const class CollisionEvent& event) {}

    void Destroy() override;
    const Transform2D* GetTransform() const { return transform; }
};