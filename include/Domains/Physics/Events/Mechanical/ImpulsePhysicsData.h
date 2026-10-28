#pragma once

#include "Vector3.h"
#include "PhysicsEvents.h"
#include "Force.h"
#include <vector>

class ImpulsePhysicsData : public AbstractPhysicsData {
private:
    std::vector<Force> forces;

public:
    ImpulsePhysicsData() = default;
    
    explicit ImpulsePhysicsData(const std::vector<Force>& forcesList) 
        : forces(forcesList) {}
    
    explicit ImpulsePhysicsData(std::vector<Force>&& forcesList) 
        : forces(std::move(forcesList)) {}

    const std::vector<Force>& GetForces() const { 
        return forces;
    }

    void SetForces(const std::vector<Force>& forcesList) {
        forces = forcesList;
    }
    
    void SetForces(std::vector<Force>&& forcesList) {
        forces = std::move(forcesList);
    }

    void AddForce(const Force& force) {
        forces.push_back(force);
    }
    
    void AddForce(Force&& force) {
        forces.push_back(std::move(force));
    }

    void Clear() {
        forces.clear();
    }

    bool HasForces() const {
        return !forces.empty();
    }
    
    size_t GetForceCount() const {
        return forces.size();
    }

    Force GetTotalForce() const {
        Force total;
        for (const auto& force : forces) {
            total = total + force;
        }
        return total;
    }
};