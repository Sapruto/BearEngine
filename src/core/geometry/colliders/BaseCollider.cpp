#include "BaseCollider.h"
#include "ColliderManager.h"
#include "CollisionEvent.h"

void BaseCollider::Destroy() {
    if (m_isBeingDestroyed) return;
    m_isBeingDestroyed = true;
    
    if (colliderManager) {
        colliderManager->RemoveCollider(this);
    }
    
    for (auto& [other, event] : currentCollisions) {
        if (other && !other->m_isBeingDestroyed) {
            if (m_isTrigger || other->m_isTrigger) {
                other->OnTriggerExit(*event);
            } else {
                other->OnCollisionExit(*event);
            }
        }
    }
    
    currentCollisions.clear();
    Component::Destroy();
}