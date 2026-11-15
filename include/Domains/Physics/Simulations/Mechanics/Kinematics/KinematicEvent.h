#pragma once

#include "../../../Events/IPhysicEvent.h"
#include "Domains/Physics/Body/PhysicalBody.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics::Kinematics {
    struct AccelerationEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId = 0;
        Vector3f acceleration;

        AccelerationEvent() = default;
        AccelerationEvent(Body::BodyId bodyId_, const Vector3f& acceleration_) : bodyId(bodyId_), acceleration(acceleration_) {}
        AccelerationEvent(const Vector3f& acceleration_) : acceleration(acceleration_) {}
    };

    struct ImpulseEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId = 0;
        Vector3f deltaVelocity;

        ImpulseEvent() = default;
        ImpulseEvent(Body::BodyId bodyId_, const Vector3f& deltaVelocity_) : bodyId(bodyId_), deltaVelocity(deltaVelocity_) {}
        ImpulseEvent(const Vector3f& deltaVelocity_) : deltaVelocity(deltaVelocity_) {}
    };
}