#include "PhysicalBody.h"

#include "GameObject.h"
#include <algorithm>

PhysicalBody::PhysicalBody(BaseCollider* collider, float density) 
    : collider(collider), density(density), mass(0), volume(0), 
      isIntialized(false), isValid(false) {
}

PhysicalBody::PhysicalBody(BaseCollider* collider) 
    : collider(collider), density(1.0f), mass(0), volume(0), 
      isIntialized(false), isValid(false) {
}

PhysicalBody::PhysicalBody(BaseCollider* collider, float mass, float volume)
    : collider(collider), mass(mass), volume(volume), density(mass/volume),
      isIntialized(false), isValid(false) {
}

PhysicalBody::~PhysicalBody() {
}

void PhysicalBody::Initialize() {
    isIntialized = false;
    isValid = false;

    if (!gameObject) return;
    
    collider = gameObject->GetComponentOfType<BaseCollider>();

    if (!collider) {
        return;
    }

    volume = collider->GetVolume();

    mass = volume * density;

    isIntialized = true;
    isValid = true;
}

void PhysicalBody::PhysicsUpdate() {

}

void PhysicalBody::RemoveFeature(PhysicFeature* feature) {
    auto it = std::find_if(features.begin(), features.end(),
        [feature](const std::unique_ptr<PhysicFeature>& f) {
            return f.get() == feature;
        });
    
    if (it != features.end()) {
        (*it)->Destroy();
        features.erase(it);
    }
}