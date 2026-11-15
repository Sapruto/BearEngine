#pragma once

#include "../../../Events/IPhysicEvent.h"
#include "../ThermalTypes.h"
#include "Domains/Physics/Body/PhysicalBody.h"

namespace Physic::Thermodynamics::Thermal {
    struct ThermalEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId = 0;
        ThermalValue changeTemperature = 0.0f;
        ThermalValue changeSpecificHeat = 0.0f;

        ThermalEvent() = default;
        ThermalEvent(ThermalValue changeTemperature_, ThermalValue changeSpecificHeat_) :
                    changeTemperature(changeTemperature_),
                    changeSpecificHeat(changeSpecificHeat_) {}
    };

    struct ThermalByEnergyEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId = 0;
        ThermalValue energy = 0.0f;

        ThermalByEnergyEvent() = default;
        ThermalByEnergyEvent(Body::BodyId bodyId_, ThermalValue energy_) :
                    bodyId(bodyId_), energy(energy_) {}
        ThermalByEnergyEvent(ThermalValue energy_) :
                    energy(energy_) {}
    };
}