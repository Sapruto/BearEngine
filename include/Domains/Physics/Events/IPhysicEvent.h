#pragma once

namespace Physic::Events {
    struct IPhysicEvent {
        IPhysicEvent() = default;
        virtual ~IPhysicEvent() = default;
    };
}