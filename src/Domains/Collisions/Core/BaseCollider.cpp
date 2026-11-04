#include "Domains/Collisions/Core/BaseCollider.h"
#include "Domains/Collisions/Core/CollisionDomain.h"
#include "Domains/Collisions/Core/CollisionEvent.h"

void BaseCollider::Destroy() {
    if (isDestroyed) return;
    isDestroyed = true;
    
    if (colliderManager) {
        colliderManager->RemoveCollider(this);
    }
    
    for (auto& [other, event] : currentCollisions) {
        if (other && !other->isDestroyed) {
            if (isTrigger || other->isTrigger) {
                other->OnTrigger(*event);
            } else {
                other->OnCollision(*event);
            }
        }
    }
    
    currentCollisions.clear();
    Component::Destroy();
}