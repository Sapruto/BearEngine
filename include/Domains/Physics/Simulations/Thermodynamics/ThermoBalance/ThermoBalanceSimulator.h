#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../SimulatorLayers.h"
#include "../../Core/MassSimulation/MassFeature.h"
#include "../Thermal/ThermalFeature.h"
#include "../Thermal/ThermalEvent.h"
#include "ThermoBalanceEvent.h"

#include <cmath>
#include <unordered_map>
#include <vector>

namespace Physic::Thermodynamics::ThermoBalance {
    class ThermoBalanceSimulator : public IPhysicSimulator {
    private:
        size_t subscriptionId = 0;
        std::unordered_map<RoomId, std::vector<Body::BodyId>> roomToBodies;

        void OnThermoBalanceEvent(const ThermoBalanceEvent& e) {
            switch (e.eventType) {
                case ThermoBalanceEvent::EventType::ADD:
                    roomToBodies[e.roomId].push_back(e.bodyId);
                    break;

                case ThermoBalanceEvent::EventType::REMOVE: {
                    auto it = roomToBodies.find(e.roomId);
                    if (it != roomToBodies.end()) {
                        std::erase(it->second, e.bodyId);
                        if (it->second.empty()) {
                            roomToBodies.erase(it);
                        }
                    }
                    break;
                }
            }
        }

    public:
        ThermoBalanceSimulator() {
            SetLayer(SimulatorLayers::Thermodynamics::THERMO_BALANCE);
        }

        void StartSimulation() override {
            auto& eventBus = GetEventBus();
            subscriptionId = eventBus.Subscribe<ThermoBalanceEvent>(
                [this](const ThermoBalanceEvent& e) { OnThermoBalanceEvent(e); },
                Events::EventDispatchMode::Immediate);
        }

        void EndFrameSimulation() override {
            auto& registry = GetBodyRegistry();
            auto& eventBus = GetEventBus();

            float deltaTime = GetTimeSystem().DeltaTime();

            for (const auto& [roomId, bodiesId] : roomToBodies) {
                if (bodiesId.size() < 2) continue;

                double totalHeatCapacity = 0.0;
                double totalThermalEnergy = 0.0;
                bool anyValid = false;

                for (auto id : bodiesId) {
                    auto* body = registry.GetBody(id);
                    if (!body) continue;

                    auto* massFeature = body->GetFeatureOfType<Mass::MassFeature>();
                    auto* thermalFeature = body->GetFeatureOfType<Thermal::ThermalFeature>();
                    if (!massFeature || !thermalFeature) continue;

                    double heatCapacity = massFeature->GetMass() * thermalFeature->specificHeat;
                    if (heatCapacity <= 0.0) continue;

                    anyValid = true;
                    totalHeatCapacity += heatCapacity;
                    totalThermalEnergy += heatCapacity * thermalFeature->temperature;
                }

                if (!anyValid || totalHeatCapacity <= 0.0) continue;

                double equilibriumTemp = totalThermalEnergy / totalHeatCapacity;
                constexpr float heatTransferRate = 0.5f;
                double factor = 1.0 - std::exp(-heatTransferRate * deltaTime);

                for (auto id : bodiesId) {
                    auto* body = registry.GetBody(id);
                    if (!body) continue;

                    auto* thermalFeature = body->GetFeatureOfType<Thermal::ThermalFeature>();
                    if (!thermalFeature) continue;

                    double delta = (equilibriumTemp - thermalFeature->temperature) * factor;
                    eventBus.AddEvent(std::make_unique<Thermal::ThermalEvent>(id, delta));
                }
            }
        }

        void DestroySimulation() override {
            if (subscriptionId != 0) {
                GetEventBus().Unsubscribe<ThermoBalanceEvent>(subscriptionId);
                subscriptionId = 0;
            }
        }
    };
}