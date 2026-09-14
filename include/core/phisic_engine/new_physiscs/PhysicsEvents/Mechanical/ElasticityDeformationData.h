#pragma once

#include "PhysicsEvents.h"
#include "Vector3.h"
#include <unordered_map>

class ElasticityDeformationData : public AbstractPhysicsData {
private:
    std::unordered_map<int, Vector3f> changedBindingPoints;

public:
    const std::unordered_map<int, Vector3f>& GetChangedBindingPoints() const { 
        return changedBindingPoints;
    }

    void SetChangedBindingPoints(const std::unordered_map<int, Vector3f>& points) {
        changedBindingPoints = points;
    }
};