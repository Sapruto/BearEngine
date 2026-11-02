#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../Core/MassSimulation/MassFeature.h"
#include "../../SimulatorLayers.h"
#include "../Kinematics/KinematicEvent.h"
#include "DynamicFeature.h"
#include "DynamicEvent.h"

namespace Physic::Mechanics::Dynamics {
    class DynamicSimulator : public IPhysicSimulator {
    private:
        size_t subscriptionId = 0;

        void OnDynamicEvent(const DynamicEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;
            
            auto feature = body->GetFeatureOfType<DynamicFeature>();
            if (!feature) return;
            
            feature->force += e.changeForce;
        }

    public:
        DynamicSimulator() { SetLayer(SimulatorLayers::Mechanics::DYNAMICS); }

        void StartSimulation() override {
            subscriptionId = GetEventBus().Subscribe<DynamicEvent>(
                [this](const DynamicEvent& e) { OnDynamicEvent(e); },
                Events::EventDispatchMode::Immediate
            );
        }

        void BeginFrameSimulation() override {
            const std::vector<Body::PhysicalBody*>& bodies = GetBodyRegistry().GetAllBodies();

            for (auto* body : bodies) {
                if (!body) continue;

                auto* dynamicFeature = body->GetFeatureOfType<DynamicFeature>();
                if (!dynamicFeature) continue;
                dynamicFeature->force = Vector3f::Zero();
            }
        }

        void EndFrameSimulation() override {
            const std::vector<Body::PhysicalBody*>& bodies = GetBodyRegistry().GetAllBodies();

            for (auto* body : bodies) {
                if (!body) continue;

                auto* dynamicFeature = body->GetFeatureOfType<DynamicFeature>();
                if (!dynamicFeature) continue;

                auto* massFeature = body->GetFeatureOfType<Mass::MassFeature>();
                if (!massFeature) continue;
                if (massFeature->GetMass() == 0) continue;

                auto acceleration = dynamicFeature->force / massFeature->GetMass();
                GetEventBus().AddEvent(std::make_unique<Kinematics::KinematicEvent>(body->GetId(), acceleration));
            }
        }

        void DestroySimulation() override {
            if (subscriptionId != 0) {
                GetEventBus().Unsubscribe<DynamicEvent>(subscriptionId);
                subscriptionId = 0;
            }
        }
    };
}