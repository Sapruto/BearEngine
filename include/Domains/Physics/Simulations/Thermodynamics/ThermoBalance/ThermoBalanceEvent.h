#pragma once

#include "../ThermalTypes.h"
#include "../../../Events/IPhysicEvent.h"
#include "Domains/Physics/Body/PhysicalBody.h"

namespace Physic::Thermodynamics::ThermoBalance {
    using RoomId = uint32_t;

    struct ThermoBalanceEvent : public Events::IPhysicEvent {
        enum class EventType {
            ADD,
            REMOVE
        };

        Body::BodyId bodyId = 0;
        RoomId roomId = 0;
        EventType eventType = EventType::ADD;

        ThermoBalanceEvent() = default;
    };
}