#pragma once

#include "include/core/phisic_engine/new_physiscs/PhysicalBody/PhysicsBody.h"

PhysicalBody::PhysicalBody(Collider collider, float mass, float volume){
    this->collider = collider;
    this->mass = mass;
    this->volume = volume;
}

void PhysicalBody::RemoveFeature(Feature* feature){
    auto it = std::find_if(features.begin(), features.end(),
        [feature](const std::unique_ptr<Feature>& f) {
            return f.get() == feature;
        });
    
    if (it != features.end()) {
        (*it)->Destroy();
        features.erase(it);
    }
}