#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../Core/TransformSimulation/TransformEvent.h"
#include "../../Core/TransformSimulation/TransformFeature.h"
#include "KinematicFeature.h"
#include "KinematicEvent.h"

namespace Physic::Mechanics {
    class KinematicSimulator : public IPhysicSimulator {
    private:
        std::unordered_map<KinematicFeature*, KinematicEvent> pending;
        size_t subscriptionId = 0;

        void OnKinematicEvent(const KinematicEvent& e) {
            if (!e.feature) return;

            auto& slot = pending[e.feature];
            slot.feature = e.feature;
            slot.acceleration = slot.acceleration + e.acceleration;
        }

    public:
        void StartSimulation() override {
            subscriptionId = GetEventBus().Subscribe<KinematicEvent>(
                [this](const KinematicEvent& e) { OnKinematicEvent(e); },
                Events::EventDispatchMode::Immediate
            );
        }

        void BeginFrameSimulation(const std::vector<Body::PhysicalBody*>& bodies) override {
            for (auto* body : bodies) {
                if (!body) continue;

                auto* feature = body->GetFeatureOfType<KinematicFeature>();
                feature->velocity += pending[feature].acceleration;
            }
        }

        void EndFrameSimulation(const std::vector<Body::PhysicalBody*>& bodies) override {
            for (auto* body : bodies) {
                if (!body) continue;

                auto* transformFeature = body->GetFeatureOfType<Transform::TransformFeature>();
                auto* kinematicFeature = body->GetFeatureOfType<KinematicFeature>();
                GetEventBus().AddEvent(std::make_unique<Transform::TransformEvent>(transformFeature, kinematicFeature->velocity * GetTimeSystem().DeltaTime()));
            }
        }

        void DestroySimulation() override {
            if (subscriptionId != 0) {
                GetEventBus().Unsubscribe<KinematicEvent>(subscriptionId);
                subscriptionId = 0;
            }
            pending.clear();
        }
    };
}