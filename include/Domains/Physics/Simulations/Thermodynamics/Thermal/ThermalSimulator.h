#pragma once

#include "../../../Core/IPhysicSimulator.h"
#include "../../SimulatorLayers.h"
#include "Domains/Physics/Simulations/Core/MassSimulation/MassFeature.h"
#include "ThermalFeature.h"
#include "ThermalEvent.h"

namespace Physic::Thermodynamics::Thermal {
    class ThermalSimulator : public IPhysicSimulator {
    private:
        size_t subscriptionIdThermal = 0;
        size_t subscriptionIdThermalByEnergy = 0;

        void OnThermalEvent(const ThermalEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;
            
            auto feature = body->GetFeatureOfType<ThermalFeature>();
            if (!feature) return;
            
            feature->temperature += e.changeTemperature;
            feature->specificHeat += e.changeSpecificHeat;
        }

        void OnThermalEventByEnergy(const ThermalByEnergyEvent& e) {
            if (e.bodyId == 0) return;

            auto body = GetBodyRegistry().GetBody(e.bodyId);
            if (!body) return;
            
            auto thermalFeature = body->GetFeatureOfType<ThermalFeature>();
            if (!thermalFeature) return;

            auto massFeature = body->GetFeatureOfType<Mass::MassFeature>();
            if (!massFeature) return;
            
            thermalFeature->temperature += e.energy / (massFeature->GetMass() * thermalFeature->specificHeat);
        }

    public:
        ThermalSimulator() { SetLayer(SimulatorLayers::Thermodynamics::THERMAL); }

        void StartSimulation() override {
            auto& eventBus = GetEventBus();

            subscriptionIdThermal = eventBus.Subscribe<ThermalEvent>(
                [this](const ThermalEvent& e) { OnThermalEvent(e); },
                Events::EventDispatchMode::Immediate);
            subscriptionIdThermalByEnergy = eventBus.Subscribe<ThermalByEnergyEvent>(
                [this](const ThermalByEnergyEvent& e) { OnThermalEventByEnergy(e); },
                Events::EventDispatchMode::Immediate);
        }

        void DestroySimulation() override {
            if (subscriptionIdThermal != 0) {
                GetEventBus().Unsubscribe<ThermalEvent>(subscriptionIdThermal);
                subscriptionIdThermal = 0;
            }
            if (subscriptionIdThermalByEnergy != 0) {
                GetEventBus().Unsubscribe<ThermalByEnergyEvent>(subscriptionIdThermalByEnergy);
                subscriptionIdThermalByEnergy = 0;
            }
        }
    };
}