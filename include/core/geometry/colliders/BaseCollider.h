#pragma once

#include "Component.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "CollisionEvent.h"
#include "CollisionReaction.h"

class ColliderManager;
class PhysicalBody;

class BaseCollider : public Component{
private:
    bool isInitialzed{false};

protected:
    ColliderManager* colliderManager{nullptr};
    std::unordered_map<BaseCollider*, const CollisionEvent*> currentCollisions;
    bool m_isBeingDestroyed{false}; 

    bool m_isTrigger{false};
    bool m_isEnabled{true};
    std::string m_tag;

    std::unique_ptr<CollisionReaction> m_ReactionCollision;
    std::unique_ptr<CollisionReaction> m_ReactionTrigger;

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

    void Initialize(){
        isInitialzed = true;
        m_ReactionCollision = std::make_unique<CollisionReaction>();
        m_ReactionTrigger = std::make_unique<CollisionReaction>();  
    }

    virtual bool Intersects(const BaseCollider* other) const = 0;

    void OnCollision(const CollisionEvent& event) { 
        if (m_ReactionCollision) {
            m_ReactionCollision->Trigger(event.GetState(), 
                                        event.GetSelf(), 
                                        event.GetOther()); 
        }
    }

    void OnTrigger(const CollisionEvent& event) { 
        if (m_ReactionTrigger) {
            m_ReactionTrigger->Trigger(event.GetState(), 
                                    event.GetSelf(), 
                                    event.GetOther()); 
        }
    }

    virtual float GetVolume() const = 0; 

    void Destroy() override;

    PhysicalBody* GetPhysicalBody() const;

    CollisionReaction* GetCollisionReaction() { return m_ReactionCollision.get(); }
    CollisionReaction* GetTriggerReaction() { return m_ReactionTrigger.get(); }

    void SubscribeToCollision(CollisionEvent::State state, 
                            CollisionReaction::EventHandler handler) {
        if (m_ReactionCollision) {
            m_ReactionCollision->Subscribe(std::move(handler), state);
        }
        else{
            Initialize();
        }
    }

    void SubscribeToTrigger(CollisionEvent::State state, 
                        CollisionReaction::EventHandler handler) {
        if (m_ReactionTrigger) {
            m_ReactionTrigger->Subscribe(std::move(handler), state);
        }
        else{
            Initialize();
        }
    }
};