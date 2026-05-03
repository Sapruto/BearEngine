#include "PhysicalBody.h"

#include "PhysicFeature.h" 
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
    Destroy();
}

void PhysicalBody::Resort(){
    sortedFeatures.clear();
    sortedFeatures.reserve(features.size());
    
    for (const auto& feature : features) {
        sortedFeatures.push_back(feature.get());
    }

    std::sort(sortedFeatures.begin(), sortedFeatures.end(),
        [](const PhysicFeature* a, const PhysicFeature* b) {
            return a->GetLayer() < b->GetLayer();
        });
    
    needsResort = false;
}

void PhysicalBody::Initialize() {
    isIntialized = false;
    isValid = false;

    if (!gameObject) return;

    transform = gameObject->GetComponentOfType<Transform3D>();

    if (collider) {
        volume = collider->GetVolume();
    }
    else {
        volume = 1.0f;
    }

    mass = volume * density;

    isIntialized = true;
    isValid = true;

    if(needsResort) Resort();

    for(auto* feature : sortedFeatures){
        feature->Initialize();
        feature->FeatureInitialize();
    }
}

void PhysicalBody::PhysicsUpdate() {
    if(!isValid) return;
    
    if(needsResort) Resort();
    
    for(auto* feature : sortedFeatures){
        feature->UpdateBody();
    }
}

void PhysicalBody::Destroy(){
    if(isDestroyed) return;

    if(needsResort) Resort();
    
    for(auto it = sortedFeatures.rbegin(); it != sortedFeatures.rend(); ++it) {
        (*it)->FeatureDestroy();
        (*it)->Destroy();
    }
    isDestroyed = true;
}

void PhysicalBody::RemoveFeature(PhysicFeature* feature) {
    auto it = std::find_if(features.begin(), features.end(),
        [feature](const std::unique_ptr<PhysicFeature>& f) {
            return f.get() == feature;
        });
    
    if (it != features.end()) {
        (*it)->Destroy();
        (*it)->FeatureDestroy();
        features.erase(it);
        needsResort = true;
    }
}