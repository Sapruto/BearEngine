#pragma once

#include "PhysicsEvents.h"
#include "Vector3.h"
#include <unordered_map>

class ElasticityDeformationData : public AbstractPhysicsData {
private:
    std::unordered_map<int, Vector3> changedBindingPoints;

public:
    const std::unordered_map<int, Vector3>& GetChangedBindingPoints() const { 
        return changedBindingPoints;
    }

    void SetChangedBindingPoints(const std::unordered_map<int, Vector3>& points) {
        changedBindingPoints = points;
    }
};