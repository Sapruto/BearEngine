#include "BaseCollider.h"
#include "ColliderManager.h"
#include "CollisionEvent.h"
#include "PhysicalBody.h"

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

PhysicalBody* BaseCollider::GetPhysicalBody() const {
    return gameObject ? gameObject->GetComponentOfType<PhysicalBody>() : nullptr;
}