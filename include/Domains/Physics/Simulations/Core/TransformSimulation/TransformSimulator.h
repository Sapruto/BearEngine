#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../SimulatorLayers.h"
#include "Math/Transform/Transform3D.h"
#include "TransformFeature.h"
#include "TransformEvent.h"

#include <unordered_map>
#include <vector>

namespace Physic::Transform {
    class TransformSimulator : public IPhysicSimulator {
    private:
        std::unordered_map<Body::BodyId, TransformEvent> pending;
        size_t subscriptionId = 0;

        void OnTransformEvent(const TransformEvent& e) {
            if (e.bodyId == 0) return;

            auto& slot = pending[e.bodyId];
            slot.changePosition = slot.changePosition + e.changePosition;
            slot.changeScale = slot.changeScale + e.changeScale;
            slot.changeRotation = slot.changeRotation * e.changeRotation;
        }

    public:
        TransformSimulator() { SetLayer(SimulatorLayers::CORE); }

        void StartSimulation() override {
            subscriptionId = GetEventBus().Subscribe<TransformEvent>(
                [this](const TransformEvent& e) { OnTransformEvent(e); },
                Events::EventDispatchMode::Immediate);
        }

        void BeginFrameSimulation() override {
            const std::vector<Body::PhysicalBody*>& bodies = GetBodyRegistry().GetAllBodies();

            for (auto* body : bodies) {
                if (!body) continue;

                auto* feature = body->GetFeatureOfType<TransformFeature>();
                auto* transform = body->GetGameObject()->GetComponentOfType<Transform3D>();
                if (!feature || !transform) continue;

                feature->position = transform->GetGlobalPosition();
                feature->scale = transform->GetGlobalScale();
                feature->rotation = transform->GetGlobalRotation();
            }
        }

        void EndFrameSimulation() override {
            const std::vector<Body::PhysicalBody*>& bodies = GetBodyRegistry().GetAllBodies();

            for (auto* body : bodies) {
                if (!body) continue;

                auto* feature = body->GetFeatureOfType<TransformFeature>();
                auto* transform = body->GetGameObject()->GetComponentOfType<Transform3D>();
                if (!feature || !transform) continue;

                auto it = pending.find(body->GetId());
                if (it != pending.end()) {
                    const auto& e = it->second;
                    feature->position = feature->position + e.changePosition;
                    feature->scale = feature->scale + e.changeScale;
                    feature->rotation = (e.changeRotation * feature->rotation).normalized();
                    pending.erase(it);
                }

                transform->SetGlobalPositionAsLocal(feature->position);
                transform->SetGlobalScaleAsLocal(feature->scale);
                transform->SetGlobalRotationAsLocal(feature->rotation);
            }

            pending.clear();
        }

        void DestroySimulation() override {
            if (subscriptionId != 0) {
                GetEventBus().Unsubscribe<TransformEvent>(subscriptionId);
                subscriptionId = 0;
            }
            pending.clear();
        }
    };
}