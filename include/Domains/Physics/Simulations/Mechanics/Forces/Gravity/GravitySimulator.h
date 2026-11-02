#pragma once

#include "../../../SimulatorLayers.h"
#include "../../Dynamics/DynamicEvent.h"
#include "Domains/Physics/Core/IPhysicSimulator.h"
#include "Domains/Physics/Simulations/Core/MassSimulation/MassFeature.h"
#include "GravityFeature.h"

namespace Physic::Mechanics::Forces::Gravity {
    class GravitySimulator : public IPhysicSimulator {
    public:
        GravitySimulator() { SetLayer(SimulatorLayers::Mechanics::Forces::GRAVITY); }

        void EndFrameSimulation() override {
            auto bodies = GetBodyRegistry().GetAllBodies();
            auto& eventBus = GetEventBus();

            for (auto body : bodies) {
                if (!body) continue;

                auto gravityFeature = body->GetFeatureOfType<GravityFeature>();
                if (!gravityFeature) continue;

                auto massFeature = body->GetFeatureOfType<Mass::MassFeature>();
                if (!massFeature) continue;

                auto changeForce = gravityFeature->g * massFeature->GetMass();
                eventBus.AddEvent(std::make_unique<Dynamics::DynamicEvent>(body->GetId(), changeForce));
            }
        }
    };
}