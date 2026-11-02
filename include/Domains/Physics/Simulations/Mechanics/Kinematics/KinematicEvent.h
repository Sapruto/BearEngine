#pragma once

#include "../../../Events/IPhysicEvent.h"
#include "Domains/Physics/Body/PhysicalBody.h"
#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics::Kinematics {
    struct KinematicEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId = 0;
        Vector3f acceleration;

        KinematicEvent() = default;
        KinematicEvent(Body::BodyId bodyId_, const Vector3f& acceleration_) : bodyId(bodyId_), acceleration(acceleration_) {}
        KinematicEvent(const Vector3f& acceleration_) : acceleration(acceleration_) {}
    };
}