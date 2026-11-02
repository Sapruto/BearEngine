#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../Core/TransformSimulation/TransformEvent.h"
#include "../../SimulatorLayers.h"
#include "Domains/Physics/Body/PhysicalBody.h"
#include "KinematicFeature.h"
#include "KinematicEvent.h"

namespace Physic::Mechanics::Kinematics {
    class KinematicSimulator : public IPhysicSimulator {
    private:
        size_t subscriptionId = 0;

        void OnKinematicEvent(const KinematicEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;
            
            auto feature = body->GetFeatureOfType<KinematicFeature>();
            if (!feature) return;
            
            feature->velocity += (e.acceleration * GetTimeSystem().DeltaTime());
        }

    public:
        KinematicSimulator() { SetLayer(SimulatorLayers::Mechanics::KINEMATICS); }

        void StartSimulation() override {
            subscriptionId = GetEventBus().Subscribe<KinematicEvent>(
                [this](const KinematicEvent& e) { OnKinematicEvent(e); },
                Events::EventDispatchMode::Immediate
            );
        }

        void EndFrameSimulation() override {
            const std::vector<Body::PhysicalBody*>& bodies = GetBodyRegistry().GetAllBodies();
            auto& eventBus = GetEventBus();

            for (auto* body : bodies) {
                if (!body) continue;

                auto* kinematicFeature = body->GetFeatureOfType<KinematicFeature>();
                if (!kinematicFeature) continue;

                auto distance = kinematicFeature->velocity * GetTimeSystem().DeltaTime();
                eventBus.AddEvent(std::make_unique<Transform::TransformEvent>(body->GetId(), distance));
            }
        }

        void DestroySimulation() override {
            if (subscriptionId != 0) {
                GetEventBus().Unsubscribe<KinematicEvent>(subscriptionId);
                subscriptionId = 0;
            }
        }
    };
}