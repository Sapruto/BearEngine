#pragma once

#include "../../../Events/IPhysicEvent.h"
#include "../../../Body/PhysicalBody.h"

namespace Physic::Mass {
    enum class MassQuantity { Mass, Volume, Density };

    struct MassEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId;
        
        MassQuantity quantity;
        float newValue;
    };
}