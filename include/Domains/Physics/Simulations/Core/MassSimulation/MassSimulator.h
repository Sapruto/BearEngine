#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../SimulatorLayers.h"
#include "MassFeature.h"
#include "MassEvent.h"

#include "Domains/Collisions/Core/BaseCollider.h"
#include "Logger/Logger.h"

#include <unordered_set>

namespace Physic::Mass {
    class MassSimulator : public IPhysicSimulator {
    private:
        size_t subscriptionId = 0;
        std::unordered_set<Body::PhysicalBody*> warnedBodies;

        void OnMassEvent(const MassEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;

            auto feature = body->GetFeatureOfType<MassFeature>();

            switch (e.quantity) {
            case MassQuantity::Mass:
                feature->SetMass(e.newValue);
                break;
            case MassQuantity::Volume:
                feature->SetVolume(e.newValue);
                break;
            case MassQuantity::Density:
                feature->SetDensity(e.newValue);
                break;
            }
        }

    public:
        MassSimulator() { SetLayer(SimulatorLayers::CORE); }

        void StartSimulation() override {
            subscriptionId = GetEventBus().Subscribe<MassEvent>(
                [this](const MassEvent& e) { OnMassEvent(e); },
                Events::EventDispatchMode::Immediate);
        }

        void BeginFrameSimulation() override {
            const std::vector<Body::PhysicalBody*>& bodies = GetBodyRegistry().GetAllBodies();

            for (auto* body : bodies) {
                if (!body) continue;

                auto* feature = body->GetFeatureOfType<MassFeature>();
                if (!feature) continue;
                if (!feature->IsVolumeFromCollider()) continue;

                auto* poly = body->GetGameObject()
                                ->GetComponentOfType<BaseCollider>();
                if (!poly) {
                    if (warnedBodies.insert(body).second) {
                        Logging::LoggerFacade::Warn(
                            "MassSimulator: isVolumeFromCollider=true, "
                            "but Polyhedron3D is missing");
                    }
                    continue;
                }

                feature->SetColliderVolume(poly->GetVolume());
            }
        }

        void DestroySimulation() override {
            if (subscriptionId != 0) {
                GetEventBus().Unsubscribe<MassEvent>(subscriptionId);
                subscriptionId = 0;
            }
            warnedBodies.clear();
        }
    };
}