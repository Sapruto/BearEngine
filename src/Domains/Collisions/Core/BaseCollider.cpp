#include "Domains/Collisions/Core/BaseCollider.h"
#include "Domains/Collisions/Core/ColliderManager.h"
#include "Domains/Collisions/Core/CollisionEvent.h"

void BaseCollider::Destroy() {
    if (m_isBeingDestroyed) return;
    m_isBeingDestroyed = true;
    
    if (colliderManager) {
        colliderManager->RemoveCollider(this);
    }
    
    for (auto& [other, event] : currentCollisions) {
        if (other && !other->m_isBeingDestroyed) {
            if (m_isTrigger || other->m_isTrigger) {
                other->OnTrigger(*event);
            } else {
                other->OnCollision(*event);
            }
        }
    }
    
    currentCollisions.clear();
    Component::Destroy();
}