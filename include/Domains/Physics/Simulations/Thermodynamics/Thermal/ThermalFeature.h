#pragma once

#include "../../../Body/IPhysicFeature.h"
#include "../ThermalTypes.h"

namespace Physic::Thermodynamics::Thermal {
    struct ThermalFeature : public Body::IPhysicFeature {
        ThermalValue temperature = 293.0f;
        ThermalValue specificHeat = 1.0f;

        ThermalFeature() = default;
        ThermalFeature(ThermalValue temperature_, ThermalValue specificHeat_) : temperature(temperature_), specificHeat(specificHeat_) {}
    };
}