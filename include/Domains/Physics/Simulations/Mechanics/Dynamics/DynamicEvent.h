#pragma once

#include "../../../Events/IPhysicEvent.h"
#include "../../../Body/PhysicalBody.h"

#include "Math/Vector/Vector3.h"

namespace Physic::Mechanics::Dynamics {
    struct DynamicEvent : public Events::IPhysicEvent {
        Body::BodyId bodyId;

        Vector3f changeForce;

        DynamicEvent() = default;
        DynamicEvent(Body::BodyId bodyId_, const Vector3f& changeForce_) : bodyId(bodyId_), changeForce(changeForce_) {}
        DynamicEvent(const Vector3f& changeForce_) : changeForce(changeForce_) {}
    };
}