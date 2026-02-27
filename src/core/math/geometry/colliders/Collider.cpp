#include "include/core/math/geometry/colliders/Collider.h"

#include "include/core/math/geometry/colliders_manager/CollisionEvent.h"
#include "include/core/math/geometry/colliders_manager/ColliderManager.h"

Collider::~Collider() {
    Destroy();
}

Collider::Collider(Collider&& other) noexcept 
    : Component(std::move(other))
    , colliderManager(other.colliderManager)
    , currentCollisions(std::move(other.currentCollisions))
    , m_isTrigger(other.m_isTrigger)
    , m_isEnabled(other.m_isEnabled)
    , m_tag(std::move(other.m_tag))
    , transform(other.transform)
{
    other.colliderManager = nullptr;
    other.transform = nullptr;
    if (colliderManager) {
        colliderManager->RemoveCollider(&other);
        colliderManager->AddCollider(this);
    }
}

Collider& Collider::operator=(Collider&& other) noexcept {
    if (this != &other) {
        if (colliderManager) {
            colliderManager->RemoveCollider(this);
        }
        
        Component::operator=(std::move(other));
        colliderManager = other.colliderManager;
        currentCollisions = std::move(other.currentCollisions);
        m_isTrigger = other.m_isTrigger;
        m_isEnabled = other.m_isEnabled;
        m_tag = std::move(other.m_tag);
        transform = other.transform;
        
        other.colliderManager = nullptr;
        other.transform = nullptr;
        
        if (colliderManager) {
            colliderManager->RemoveCollider(&other);
            colliderManager->AddCollider(this);
        }
    }
    return *this;
}

void Collider::Destroy(){
    if (m_isBeingDestroyed) return;
    m_isBeingDestroyed = true;
    
    if (colliderManager) {
        colliderManager->RemoveCollider(this);
    }
}