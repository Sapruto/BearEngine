#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "World/Components/Component.h"
#include "CollisionEvent.h"
#include "CollisionReaction.h"

class CollisionDomain;

class BaseCollider : public Component {
private:
    bool isInitialzed{false};

    CollisionDomain* colliderManager{nullptr};
    std::unordered_map<BaseCollider*, const CollisionEvent*> currentCollisions;
    bool isDestroyed{false}; 

    bool isTrigger{false};
    bool isEnabled{true};
    std::string tag;

    std::unique_ptr<CollisionReaction> reactionCollision;
    std::unique_ptr<CollisionReaction> reactionTrigger;

public:
    BaseCollider() = default;
    virtual ~BaseCollider() = default; 

    void SetCollisionDomain(CollisionDomain* manager) { colliderManager = manager; }
    CollisionDomain* GetCollisionDomain() const { return colliderManager; }

    void SetIsTrigger(bool isTrigger_) { isTrigger = isTrigger_; }
    bool IsTrigger() const { return isTrigger; }

    void SetEnabled(bool enabled) { isEnabled = enabled; }
    bool IsEnabled() const { return isEnabled; }

    void SetTag(const std::string& tag_) { tag = tag_; }
    const std::string& GetTag() const { return tag; }

    std::unordered_map<BaseCollider*, const CollisionEvent*>& GetCurrentCollisions() { 
        return currentCollisions; 
    }
    const auto& GetCurrentCollisions() const { return currentCollisions; }
    
    bool IsCollidingWith(const BaseCollider* other) const { 
        return currentCollisions.find(const_cast<BaseCollider*>(other)) != currentCollisions.end();
    }
    
    void AddCollision(BaseCollider* other, const CollisionEvent* event) {
        currentCollisions[other] = event;
    }
    
    void RemoveCollision(BaseCollider* other) {
        currentCollisions.erase(other);
    }
    
    void ClearCollisions() {
        currentCollisions.clear();
    }

    void Initialize(){
        isInitialzed = true;
        reactionCollision = std::make_unique<CollisionReaction>();
        reactionTrigger = std::make_unique<CollisionReaction>();  
    }

    virtual bool Intersects(const BaseCollider* other) const = 0;

    void OnCollision(const CollisionEvent& event) { 
        if (reactionCollision) {
            reactionCollision->Trigger(event.GetState(), 
                                        event.GetSelf(), 
                                        event.GetOther()); 
        }
    }

    void OnTrigger(const CollisionEvent& event) { 
        if (reactionTrigger) {
            reactionTrigger->Trigger(event.GetState(), 
                                    event.GetSelf(), 
                                    event.GetOther()); 
        }
    }

    virtual float GetVolume() const = 0; 

    void Destroy() override;

    CollisionReaction* GetCollisionReaction() { return reactionCollision.get(); }
    CollisionReaction* GetTriggerReaction() { return reactionTrigger.get(); }

    void SubscribeToCollision(CollisionEvent::State state, 
                            CollisionReaction::EventHandler handler) {
        if (reactionCollision) {
            reactionCollision->Subscribe(std::move(handler), state);
        }
        else{
            Initialize();
        }
    }

    void SubscribeToTrigger(CollisionEvent::State state, 
                        CollisionReaction::EventHandler handler) {
        if (reactionTrigger) {
            reactionTrigger->Subscribe(std::move(handler), state);
        }
        else{
            Initialize();
        }
    }
};