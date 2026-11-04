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
        size_t subscriptionAccelerationId = 0;
        size_t subscriptionImpulseId = 0;

        void OnAccelerationEvent(const AccelerationEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;
            
            auto feature = body->GetFeatureOfType<KinematicFeature>();
            if (!feature) return;
            
            feature->velocity += (e.acceleration * GetTimeSystem().DeltaTime());
        }

        void OnImpulseEvent(const ImpulseEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;
            
            auto feature = body->GetFeatureOfType<KinematicFeature>();
            if (!feature) return;
            
            feature->velocity += e.deltaVelocity;
        }

    public:
        KinematicSimulator() { SetLayer(SimulatorLayers::Mechanics::KINEMATICS); }

        void StartSimulation() override {
            subscriptionAccelerationId = GetEventBus().Subscribe<AccelerationEvent>(
                [this](const AccelerationEvent& e) { OnAccelerationEvent(e); },
                Events::EventDispatchMode::Immediate
            );
            subscriptionImpulseId = GetEventBus().Subscribe<ImpulseEvent>(
                [this](const ImpulseEvent& e) { OnImpulseEvent(e); },
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
            if (subscriptionAccelerationId != 0) {
                GetEventBus().Unsubscribe<AccelerationEvent>(subscriptionAccelerationId);
                subscriptionAccelerationId = 0;
            }
            if (subscriptionImpulseId != 0) {
                GetEventBus().Unsubscribe<ImpulseEvent>(subscriptionImpulseId);
                subscriptionImpulseId = 0;
            }
        }
    };
}